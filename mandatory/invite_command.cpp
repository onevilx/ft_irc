#include "../headers/channel.hpp"
#include "../headers/client.hpp"
#include "../headers/server.hpp"
#include "../headers/replies.hpp"


std::vector<Client *>& Channel::get_inv(){
    return this->invited;
}
void    Server::inv(Client *client, Commands cmd){
    std::vector<std::string> arg = cmd.getArgs();
    // so the logic here will arse this INVITE nick_name channel

    // arg[1] == nicknaem;
    // arg[2] == channel_

    if(arg.empty() || arg.size() < 2){
        send(client->getFd(), ERROR_NEEDMOREPARAMS(client->getNickname(), client->getHostname()).c_str(), \
        ERROR_NEEDMOREPARAMS(client->getNickname(), client->getHostname()).length(), 0);
        return;
    }
    Client *tar = this->findClientByNick(arg[0]);
    if(!tar){
        std::string msg =  ERROR_NOSUCHNICK(client->getHostname(), arg[1], arg[0]); 
        send(client->getFd(), msg.c_str(), msg.length(), 0);
        return ;
    }
    Channel *cl = this->get_single_channel(arg[1]);
    if(!cl){
        std::string msg =  ERROR_NOSUCHCHANNEL(client->getHostname(), arg[1], client->getNickname());
        send(client->getFd(), msg.c_str(), msg.length(), 0);
        return;
    }
    cl->get_inv().push_back(tar);
    std::string msg =  REPLY_INVITE(client->getNickname(), client->getUsername(), client->getHostname(), tar->getNickname(),cl->get_Cname());
    send(tar->getFd(), msg.c_str(), msg.length(), 0);
    std::string msg1  = REPLY_INVITING(client->getHostname(), client->getNickname(), tar->getNickname(), cl->get_Cname());
    send(client->getFd(), msg1.c_str(), msg1.length(), 0);



}