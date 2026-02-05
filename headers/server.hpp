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

public:
    Server(int port, const std::string& password);
    ~Server();

    int                         getServerFd() const;
    int                         getPort() const;
    const std::string&          getPassword() const;
    std::map<int, Client*>&     getClients();
    void    run();
};

#endif
