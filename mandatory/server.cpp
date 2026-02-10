#include "../headers/server.hpp"
#include "../headers/client.hpp"
#include "../headers/commands.hpp"

// -------------------- Helpers --------------------

std::string Server::toUpper(const std::string& s)
{
    std::string out = s;
    for (size_t i = 0; i < out.size(); ++i)
        out[i] = (char)toupper((unsigned char)out[i]);
    return out;
}

std::string Server::stripTrailingColon(const std::string& s)
{
    if (!s.empty() && s[s.size() - 1] == ':')
        return s.substr(0, s.size() - 1);
    return s;
}

Client* Server::findClientByNick(const std::string& nick) const
{
    if (nick.empty()) return NULL;
    std::string want = toUpper(nick);
    for (std::map<int, Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        Client* c = it->second;
        if (!c) continue;
        const std::string &cnick = c->getNickname();
        if (!cnick.empty() && toUpper(cnick) == want)
            return c;
    }
    return NULL;
}

// -------------------- Server Constructor / Destructor --------------------

Server::Server(int port, const std::string& password)
    : _serverFd(-1), _port(port), _password(password)
{
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

    pollfd pfd;
    pfd.fd = _serverFd;
    pfd.events = POLLIN;
    _pollFds.push_back(pfd);

    std::cout << "[SERVER] Listening on " << _port << std::endl;
}

Server::~Server()
{
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        delete it->second;
    close(_serverFd);
}

// -------------------- Main Loop --------------------

void Server::run()
{
    while (true)
    {
        if (poll(&_pollFds[0], _pollFds.size(), -1) < 0)
            throw std::runtime_error("poll() failed");

        for (size_t i = 0; i < _pollFds.size(); i++)
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

// -------------------- Client Handling --------------------

void Server::acceptNewClient()
{
    int fd = accept(_serverFd, NULL, NULL);
    if (fd < 0)
        return;

    fcntl(fd, F_SETFL, O_NONBLOCK);

    pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    _pollFds.push_back(pfd);

    _clients[fd] = new Client(fd);
    std::cout << "[CONNECT] fd " << fd << std::endl;
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

// -------------------- Command Handling --------------------

void Server::handleCommand(Client* client, Commands& cmd)
{
    if (!client->isAuthenticated())
    {
        handleAuth(client, cmd);
        return;
    }

    const std::string& command = cmd.getCommand();
    const std::vector<std::string>& args = cmd.getArgs();

    if (command == "PING")
    {
        if (!args.empty())
        {
            std::string pong = "PONG server " + args[0] + "\r\n";
            send(client->getFd(), pong.c_str(), pong.size(), 0);
        }
    }
    else if (command == "JOIN")
        join(client, cmd);
    else if (command == "PRIVMSG")
    {
        if (args.size() < 2)
        {
            std::string err = ":server 411 " + client->getNickname() + " :No recipient given\r\n";
            send(client->getFd(), err.c_str(), err.size(), 0);
            return;
        }

        std::string targetNick = stripTrailingColon(args[0]);
        std::string message = args[1];

        Client* target = findClientByNick(targetNick);
        if (!target)
        {
            std::string err = ":server 401 " + client->getNickname() + " " + targetNick + " :No such nick\r\n";
            send(client->getFd(), err.c_str(), err.size(), 0);
        }
        else
        {
            std::string msg = ":" + client->getNickname() + "!" + client->getUsername() +
                              "@localhost PRIVMSG " + targetNick + " :" + message + "\r\n";
            send(target->getFd(), msg.c_str(), msg.size(), 0);
        }
    }
    else
    {
        std::string err = ":server 421 " + client->getNickname() + " " + command + " :Unknown command\r\n";
        send(client->getFd(), err.c_str(), err.size(), 0);
    }

    std::cout << "[CMD] " << command << " from fd " << client->getFd() << std::endl;
}

// -------------------- Authentication --------------------

void Server::handleAuth(Client* client, Commands& cmd)
{
    const std::string& c = cmd.getCommand();
    const std::vector<std::string>& a = cmd.getArgs();

    if (c == "PASS")
    {
        if (a.empty() || a[0] != _password)
        {
            send(client->getFd(), "ERROR :Bad password\r\n", 22, 0);
            removeClient(client->getFd());
            return;
        }
        client->setPassOk();
    }
    else if (c == "NICK")
    {
        if (!a.empty())
        {
            std::string requested = stripTrailingColon(a[0]);
            Client* existing = findClientByNick(requested);
            if (existing && existing != client)
            {
                std::string msg = ":server 433 * " + requested + " :Nickname is already in use\r\n";
                send(client->getFd(), msg.c_str(), msg.size(), 0);
            }
            else
            {
                client->setNickname(requested);
                client->setNickOk();
            }
        }
    }
    else if (c == "USER")
    {
        if (!a.empty())
        {
            client->setUsername(a[0]);
            client->setUserOk();
        }
    }
    else
    {
        std::string msg = ":server 451 " + client->getNickname() + " :You are not registered\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return;
    }

    client->tryAuthenticate();

    if (client->isAuthenticated())
    {
        std::string welcome = ":server 001 " + client->getNickname() + " :Welcome to the IRC Network, " + client->getNickname() + "\r\n";
        send(client->getFd(), welcome.c_str(), welcome.size(), 0);
        std::cout << "[AUTH] fd " << client->getFd() << " authenticated" << std::endl;
    }
}

// -------------------- Disconnect --------------------

void Server::removeClient(int fd)
{
    for (size_t i = 0; i < _pollFds.size(); i++)
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