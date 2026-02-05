#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <map>
#include <vector>
#include <poll.h>

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

    void    run();
};

#endif