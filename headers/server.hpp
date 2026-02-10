#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <math.h>

#include <cstring>
#include <vector>
#include <poll.h>
#include <iostream>
#include <fcntl.h>
#include <arpa/inet.h>
#include <stdlib.h>

class Commands;
class Client;

class Server
{
    private:
    int                         _serverFd;
    int                         _port;
    std::string                 _password;
    std::vector<struct pollfd>  _pollFds;
    std::map<int, Client*>      _clients;

    void    acceptNewClient();
    void    receiveData(int fd);
    void    removeClient(int fd);
    void    handleCommand(Client* client, Commands& cmd);
    void    handleAuth(Client* client, Commands& cmd);
    Client* findClientByNick(const std::string& nick) const;
    static std::string toUpper(const std::string& s);
    static std::string stripTrailingColon(const std::string& s);


    public:
    Server(int port, const std::string& password);
    ~Server();

    void    run();
};

#endif
