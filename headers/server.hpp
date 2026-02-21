#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <math.h>
#include "channel.hpp"
#include "commands.hpp"
#include <cstring>
#include <vector>
#include <poll.h>
#include <iostream>
#include <fcntl.h>
#include <arpa/inet.h>
#include <stdlib.h>

struct mode_arg
{
    char *command;
    char **args;
    mode_arg *next;
};

class Commands;
class Client;
class Channel;

class Server
{
    private:
    std::string                 _creationTime;
    int                         _serverFd;
    int                         _port;
    std::string                 _password;
    std::vector<struct pollfd>  _pollFds;
    std::map<int, Client *>      _clients; // khoya chre7 liya 3lach katsavihom as map 
    std::vector< Channel * > channels; // here is the channels inside a server

    void    acceptNewClient();
    void    receiveData(int fd);
    void    removeClient(int fd);
    void    handleCommand(Client* client, Commands& cmd);
    void    handleAuth(Client* client, Commands& cmd);
    Channel* findChannel(const std::string& name);
    static std::string toUpper(const std::string& s);
    static std::string stripTrailingColon(const std::string& s);
    Channel * get_channel_by_name(std::string Cname);

    public:
    Client* findClientByNick(const std::string& nick) const;
    Server(int port, const std::string& password);
    std::vector<Channel *>& get_channels();
    ~Server();

    void    channel_msg(Client *user, const std::string msg, std::string Cname);
    void    sendToclient(int fd,const std::string& msg);
    bool    isClinetinChannel(Client *user, std::string name);
    void    initJOINReply(Client *user,Channel *channel);
    std::string Clientsnamebuilder(Channel *channel);
    Channel * get_single_channel(std::string to_fetch);
    void    run();
    //big boss
    void join(Client *client, Commands cmd);
    void mode(Client *client, Commands cmd);
    // dazai's method to validate the addition of a channel so i can look for 
    //the name of channels inside a server
    bool exists(std::string name);
    // here i will be creating channels;
    Channel* creat_channel();
    void handleTopic(Client* client, Commands& cmd);
    void handlePrivmsg(Client* client, Commands& cmd);
    void inv(Client *client, Commands cmd);
};

#endif
