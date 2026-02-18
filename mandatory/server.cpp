#include "../headers/server.hpp"
#include "../headers/client.hpp"
#include "../headers/commands.hpp"
#include "../headers/replies.hpp"

// ==========================================================
// Helpers
// ==========================================================

std::string Server::toUpper(const std::string& s)
{
    std::string out = s;
    for (size_t i = 0; i < out.size(); ++i)
        out[i] = (char)toupper((unsigned char)out[i]);
    return out;
}

std::string Server::stripTrailingColon(const std::string& s)
{
    if (!s.empty() && s[0] == ':')
        return s.substr(1);
    return s;
}

Client* Server::findClientByNick(const std::string& nick) const
{
    if (nick.empty())
        return NULL;

    std::string want = toUpper(nick);

    for (std::map<int, Client*>::const_iterator it = _clients.begin();
         it != _clients.end(); ++it)
    {
        Client* c = it->second;
        if (!c)
            continue;

        if (!c->getNickname().empty() &&
            toUpper(c->getNickname()) == want)
            return c;
    }
    return NULL;
}

// ==========================================================
// Constructor / Destructor
// ==========================================================

Server::Server(int port, const std::string& password)
    : _serverFd(-1), _port(port), _password(password)
{
    if (port < 1024 || port > 65535)
        throw std::runtime_error("invalid port, try again!");
    _serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverFd < 0)
        throw std::runtime_error("socket failed");

    int opt = 1;
    setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    fcntl(_serverFd, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(_port);

    if (bind(_serverFd, (sockaddr*)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("bind failed");
    if (listen(_serverFd, SOMAXCONN) < 0)
        throw std::runtime_error("listen failed");

    time_t now = time(NULL);
    struct tm* gmt = gmtime(&now);

    char buffer[100];
    strftime(buffer, sizeof(buffer),
             "%a, %d %b %Y %H:%M:%S UTC",
             gmt);

    _creationTime = buffer;

    pollfd pfd;
    pfd.fd = _serverFd;
    pfd.events = POLLIN;
    _pollFds.push_back(pfd);

    std::cout << "[SERVER] Listening on " << _port << std::endl;
}

Server::~Server()
{
    for (std::map<int, Client*>::iterator it = _clients.begin();
         it != _clients.end(); ++it)
        delete it->second;

    close(_serverFd);
}

// ==========================================================
// Main Loop
// ==========================================================

void Server::run()
{
    while (true)
    {
        if (poll(&_pollFds[0], _pollFds.size(), -1) < 0)
            throw std::runtime_error("poll failed");

        for (size_t i = 0; i < _pollFds.size(); ++i)
        {
            if (_pollFds[i].revents & POLLIN)
            {
                if (_pollFds[i].fd == _serverFd)
                    acceptNewClient();
                else
                    receiveData(_pollFds[i].fd);
            }
        }
    }
}

// ==========================================================
// Client Handling
// ==========================================================

void Server::acceptNewClient()
{
    sockaddr_in addr;
    socklen_t len = sizeof(addr);

    int fd = accept(_serverFd, (sockaddr*)&addr, &len);
    if (fd < 0)
        return;

    fcntl(fd, F_SETFL, O_NONBLOCK);

    // Get client IP
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));

    pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    _pollFds.push_back(pfd);

    Client* client = new Client(fd);
    client->setIpAddress(ip);
    _clients[fd] = client;

    std::cout << "[CONNECT] fd " << fd
              << " IP: " << ip << std::endl;
}

void Server::receiveData(int fd)
{
    char buffer[1024];
    ssize_t bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes <= 0)
    {
        removeClient(fd);
        return;
    }

    Client* client = _clients[fd];
    client->appendBuffer(std::string(buffer, bytes));

    while (client->hasCompleteCommand())
    {
        Commands cmd(client->extractCommand());
        handleCommand(client, cmd);
    }
}

void Server::sendToclient(int fd, const std::string& msg)
{
    send(fd, msg.c_str(), msg.length(), 0);
}

void Server::removeClient(int fd)
{
    for (size_t i = 0; i < _pollFds.size(); ++i)
    {
        if (_pollFds[i].fd == fd)
        {
            _pollFds.erase(_pollFds.begin() + i);
            break;
        }
    }

    if (_clients.count(fd))
    {
        close(fd);
        delete _clients[fd];
        _clients.erase(fd);
    }

    std::cout << "[DISCONNECT] fd " << fd << std::endl;
}

// ==========================================================
// Command Dispatcher
// ==========================================================

void Server::handleCommand(Client* client, Commands& cmd)
{
    std::string command = toUpper(cmd.getCommand());

    // ===============================
    // UNREGISTERED CLIENT
    // ===============================
    if (!client->isAuthenticated())
    {
        if (command == "PASS" ||
            command == "NICK" ||
            command == "USER")
        {
            handleAuth(client, cmd);
        }
        else
        {
            std::string err = ERROR_NOTREGISTERED(client->getNickname(), client->get_client_host());
            send(client->getFd(), err.c_str(), err.size(), 0);
        }
        return;
    }

    // ===============================
    // REGISTERED CLIENT
    // ===============================

    // NICK (change after registration)
    if (command == "NICK")
    {
        const std::vector<std::string>& args = cmd.getArgs();

        if (args.empty())
        {
            std::string msg = ERROR_NEEDMOREPARAMS(
                client->getNickname(),
                client->get_client_host()
            );
            send(client->getFd(), msg.c_str(), msg.size(), 0);
            return;
        }

        std::string newNick = stripTrailingColon(args[0]);

        Client* existing = findClientByNick(newNick);
        if (existing && existing != client)
        {
            std::string err = ERROR_NICKNAMEINUSE(
                client->getNickname(),
                client->get_client_host()
            );
            send(client->getFd(), err.c_str(), err.size(), 0);
            return;
        }

        std::string oldNick = client->getNickname();
        client->setNickname(newNick);

        std::string msg = REPLY_NICKCHANGE(
            oldNick,
            newNick,
            client->get_client_host()
        );
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return;
    }

    if (command == "JOIN")
    {
        join(client, cmd);
        return;
    }

    if (command == "PING")
    {
        const std::vector<std::string>& args = cmd.getArgs();
        if (!args.empty())
        {
            std::string pong = "PONG :" + args[0] + "\r\n";
            send(client->getFd(), pong.c_str(), pong.size(), 0);
        }
        return;
    }

    if (command == "TOPIC")
    {
        handleTopic(client, cmd);   // moved to topic.cpp
        return;
    }

    if (command == "PRIVMSG")
    {
        handlePrivmsg(client, cmd); // moved to privmsg.cpp
        return;
    }

    // UNKNOWN COMMAND
    std::string err = ERROR_UNKNOWNCOMMAND(
        client->getNickname(),
        client->get_client_host(),
        command
    );
    send(client->getFd(), err.c_str(), err.size(), 0);
}

// ==========================================================
// Channel Lookup
// ==========================================================

Channel* Server::findChannel(const std::string& name)
{
    for (size_t i = 0; i < channels.size(); ++i)
    {
        if (channels[i]->get_Cname() == name)
            return channels[i];
    }
    return NULL;
}
