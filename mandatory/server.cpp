#include "../headers/server.hpp"
#include "../headers/client.hpp"

Server::Server(int port, const std::string& password) : _serverFd(-1), _port(port), _password(password)
{
    _serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverFd < 0)
        throw std::runtime_error("socket() failed");

    int opt = 1;
    if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        throw std::runtime_error("setsockopt() failed");

    fcntl(_serverFd, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(_port);

    if (bind(_serverFd, (sockaddr*)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("bind() failed");

    if (listen(_serverFd, SOMAXCONN) < 0)
        throw std::runtime_error("listen() failed");

    pollfd pfd;
    pfd.fd = _serverFd;
    pfd.events = POLLIN;
    _pollFds.push_back(pfd);

    std::cout << "[SERVER] Listening on port " << _port << std::endl;
}

Server::~Server()
{
    for (std::map<int, Client*>::iterator it = _clients.begin();
         it != _clients.end(); ++it)
        delete it->second;
    close(_serverFd);
}

int Server::getServerFd() const { return _serverFd; }
int Server::getPort() const { return _port; }
const std::string& Server::getPassword() const { return _password; }
std::map<int, Client*>& Server::getClients() { return _clients; }

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

void Server::acceptNewClient()
{
    sockaddr_in addr;
    socklen_t len = sizeof(addr);

    int fd = accept(_serverFd, (sockaddr*)&addr, &len);
    if (fd < 0)
        return;

    fcntl(fd, F_SETFL, O_NONBLOCK);

    pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    _pollFds.push_back(pfd);

    _clients[fd] = new Client(fd);

    std::cout << "[SERVER] Client connected (fd " << fd << ")" << std::endl;
}

IRCCommand parseCommandLine(const std::string& line)
{
    IRCCommand cmd;
    std::string workingLine = line;
    if (!workingLine.empty() && workingLine.back() == '\r')
        workingLine.pop_back();

    std::istringstream iss(workingLine);
    std::string token;
    if (!(iss >> cmd.command))
        return cmd;

    // IRC commands are case-insensitive → uppercase
    std::transform(cmd.command.begin(), cmd.command.end(), cmd.command.begin(), ::toupper);
    std::string param;
    bool trailing = false;
    std::string trailingStr;
    while (iss >> param)
    {
        if (!trailing && param[0] == ':')
        {
            trailing = true;
            trailingStr = param.substr(1);
        }
        else if (trailing)
        {
            trailingStr += " " + param;
        }
        else
        {
            cmd.args.push_back(param);
        }
    }
    if (trailing)
        cmd.args.push_back(trailingStr);
    return cmd;
}

void Server::receiveData(int fd)
{
    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));

    ssize_t bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0)
    {
        removeClient(fd);
        return;
    }

    Client* client = _clients[fd];
    client->appendBuffer(buffer);

    while (client->hasCompleteCommand())
    {
        std::string line = client->extractCommand();
        IRCCommand cmd = parseCommandLine(line);

        // For now: print structured command for your teammate man ba3d ghadi ngado replies
        std::cout << "[PARSED] fd:" << client->getFd()
                << " CMD:" << cmd.command << " ARGS:";
        for (size_t i = 0; i < cmd.args.size(); i++)
            std::cout << "{" << cmd.args[i] << "}";
        std::cout << std::endl;

        // chli7a implemente commands HEREEEEEEEEEEEEEEEEEEEEEEEE: 
        // handleCommand(client, cmd);
    }
}

void Server::removeClient(int fd)
{
    std::cout << "[SERVER] Client disconnected (fd " << fd << ")" << std::endl;

    for (size_t i = 0; i < _pollFds.size(); i++)
    {
        if (_pollFds[i].fd == fd)
        {
            _pollFds.erase(_pollFds.begin() + i);
            break;
        }
    }

    delete _clients[fd];
    _clients.erase(fd);
}