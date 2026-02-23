#include "../headers/channel.hpp"
#include "../headers/client.hpp"
#include "../headers/server.hpp"
#include "../headers/replies.hpp"
#include "../headers/commands.hpp"

void Server::leave_all_channels(Client *user){
    for (std::vector<Channel *>::iterator itr1 = this->get_channels().begin(); itr1 != this->get_channels().end(); itr1++){
        for (std::vector<Client *>::iterator itr2 = (*itr1)->get_ClientsinChannel().begin(); itr2 != (*itr1)->get_ClientsinChannel().end(); itr2++)
        {
            if((*itr2)->getNickname() == user->getNickname()){
                std::string msg = PART_REPLY((*itr2)->getNickname(), (*itr2)->getUsername(), (*itr2)->getHostname(), (*itr1)->get_Cname(), "");
                (*itr1)->send_toclients(msg);
                (*itr1)->get_ClientsinChannel().erase(itr2);
                break;
            }
        }
    }
    for(std::vector<Channel *>::iterator itr1 = this->get_channels().begin(); itr1 != this->get_channels().end(); itr1++){
        for (std::vector<Client *>::iterator itr2 = (*itr1)->get_ops().begin(); itr2 != (*itr1)->get_ops().end(); itr2++)
        {
            if((*itr2)->getNickname() == user->getNickname()){
                (*itr1)->get_ops().erase(itr2);
                break;
            }
        }
    }
}

bool Server::exists(std::string name) {
    for (int i = 0; i < this->channels.size(); i++)
        if(name == this->channels[i]->get_Cname())
            return true;
    return false;
}

std::vector<std::string> split(std::string args, std::string delimiter){
    std::vector<std::string> res;
    size_t pos = 0;
    std::string tmp;
    while ((pos = args.find(delimiter)) != std::string::npos)
    {
        tmp = args.substr(0, pos);
        res.push_back(tmp);
        args.erase(0, pos + delimiter.length());
    }
    res.push_back(args);
    return res;
}

void Server::channel_msg(Client *user, const std::string msg, std::string Cname){
    for(std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it){
        if((*it)->get_Cname() == Cname)
            for (size_t i = 0; i < (*it)->get_ClientsinChannel().size(); i++)
        {

            if((*it)->get_ClientsinChannel()[i]->getNickname()  != user->getNickname())
                    sendToclient((*it)->get_ClientsinChannel()[i]->getFd(), msg);
        }
    }
}

bool    Server::isClinetinChannel(Client *user, std::string name){
        for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); it++)
        {
            if((*it)->get_Cname() == name){
            for (std::vector<Client*>::iterator iter = (*it)->get_ClientsinChannel().begin(); iter != (*it)->get_ClientsinChannel().end(); iter++)
            {
                if((*iter)->getNickname() == user->getNickname())
                    return true;
            }}
        }
        return false;
}

std::string Server::Clientsnamebuilder(Channel *channel){
    std::string reply;
    std::vector<Client *> clients = channel->get_ClientsinChannel();
    for (std::vector<Client *>::iterator iter = clients.begin(); iter != clients.end(); iter++)
    {
        Client* client = *iter; 
        if(client->get_operator()){
             if (reply.empty())
                reply += "@" + client->getNickname();
            else
                reply += " @" + client->getNickname();
        }
        else{
             if (reply.empty())
                reply += client->getNickname();
            else
                reply += " " + client->getNickname();
        }
    }
    return reply;
}

bool  is_invited(Client * client, Channel *cl){
    for (std::vector<Client *>::iterator itr = cl->get_inv().begin(); itr != cl->get_inv().end(); itr++ )
    {
        if((*itr)->getNickname() == client->getNickname())
            return true;
    }
    return false;
    
}

void Server::initJOINReply(Client *user, Channel *channel){
    std::string msg1; std::string msg2; std::string msg3; std::string msg4;
    // msg1 = REPLY_WELCOME(user->getNickname(), name);
    msg1 = REPLY_JOIN(user->getNickname(), user->getUsername(), channel->get_Cname(), user->getHostname());
    send(user->getFd(), msg1.c_str(), msg1.length(), 0);
    msg2 =  REPLY_CHANNELMODES(user->getHostname(), channel->get_Cname(), user->getNickname(), channel->get_channel_mode()); // maybe to set later cause i need modes
    send(user->getFd(), msg2.c_str(), msg2.length(), 0);
    msg3 = REPLY_NAMREPLY(user->getHostname(),  Clientsnamebuilder(channel), channel->get_Cname(), user->getNickname());
    send(user->getFd(), msg3.c_str(), msg3.length(), 0);
    msg4 = REPLY_ENDOFNAMES(user->getHostname(), user->getNickname(), channel->get_Cname());
    send(user->getFd(), msg4.c_str(), msg4.length(), 0);
} 

void Server::join(Client *client, Commands cmd){

    std::vector<std::string> args = cmd.getArgs();
   if(args.size() < 1 || args[0].empty()){
    std::string msg = ERROR_NEEDMOREPARAMS(client->getNickname(), client->getHostname());
    send(client->getFd(), msg.c_str(), msg.length(), 0);
    return;
   }
    std::stringstream ss(args[0]);
    std::string n;
    ss >> n;
    std::vector<std::string> Cnames;
    if(n == "0"){
       leave_all_channels(client);
        return;
    }
   Cnames = split(args[0], ",");
   for (std::vector<std::string>::iterator it = Cnames.begin(); it != Cnames.end(); ++it){
    if(args.size() >= 1){
        std::string name = *it;
        if(name[0] != '#' || name.empty() || name.substr(1).find(" ") != std::string::npos){
            std::string msg = ERROR_NOSUCHCHANNEL(client->get_client_host(), name, client->getNickname());
            send(client->getFd(), msg.c_str(), msg.length(), 0);
            return ;
        }
       if(exists(name)){
        Channel *cl = this->get_single_channel(name);
        if(cl->get_i() && !is_invited(client, cl)){
            std::string msg = ERROR_INVITEONLY(client->getUsername(), cl->get_Cname());
            send(client->getFd(), msg.c_str() , msg.length(), 0);
            return;
        }
        if(cl->get_l()){
            if(cl->count_users() >= cl->get_limit()){
                std::string msg = ERROR_CHANNELISFULL(client->getNickname(), cl->get_Cname());
                send(client->getFd(), msg.c_str() , msg.length(), 0);
                return;
            }
        }
            if(isClinetinChannel(client, name)){
                std::string msg = ERROR_USERONCHANNEL(client->get_client_host(), name, client->getNickname());
                send(client->getFd(), msg.c_str(), msg.length(), 0);
                continue;
            }
            else{
                if(cl->get_k())
                    {
                        if(args[1].empty()){
                           send(client->getFd(), ERROR_BADCHANNELKEY(client->getNickname(), client->getHostname(), cl->get_Cname()).c_str(), \
                           ERROR_BADCHANNELKEY(client->getNickname(), client->getHostname(), cl->get_Cname()).length(), 0);
                           return ;
                        }
                        if(args[1] == cl->get_key_val()){
                            for (std::vector<Channel*>::iterator iter1 = channels.begin(); iter1 != channels.end(); iter1++){
                            if((*iter1)->get_Cname() == name){
                            (*iter1)->addtoChannel(client, "");
                            initJOINReply(client,  (*iter1));
                            std::string msg = REPLY_JOIN(client->getNickname(), client->getUsername(), name, client->getHostname());
                            for (std::vector<Client*>::iterator c = (*iter1)->get_ClientsinChannel().begin(); c != (*iter1)->get_ClientsinChannel().end(); c++)
                                {
                                if((*c)->getFd() != client->getFd())
                                    send((*c)->getFd(), msg.c_str(), msg.length(), 0);
                                }
                            }
                            }
                            }
                        else{
                            send(client->getFd(), ERROR_BADCHANNELKEY(client->getNickname(), client->getHostname(), cl->get_Cname()).c_str(), \
                           ERROR_BADCHANNELKEY(client->getNickname(), client->getHostname(), cl->get_Cname()).length(), 0);

                        }
                        }
                else{
                for (std::vector<Channel*>::iterator iter1 = channels.begin(); iter1 != channels.end(); iter1++){
                    if((*iter1)->get_Cname() == name){
                        (*iter1)->addtoChannel(client, "");
                        initJOINReply(client,  (*iter1));
                        std::string msg = REPLY_JOIN(client->getNickname(), client->getUsername(), name, client->get_client_host());
                        for (std::vector<Client*>::iterator c = (*iter1)->get_ClientsinChannel().begin(); c != (*iter1)->get_ClientsinChannel().end(); c++)
                        {
                            if((*c)->getFd() != client->getFd())
                                send((*c)->getFd(), msg.c_str(), msg.length(), 0);
                        }
                    }
                }
            }
            }
        
       }
       else{
        Channel* newChannel = new Channel(this ,name, "");
        if(!newChannel->addtoChannel(client, ""))
            continue;  
        channels.push_back(newChannel);
        newChannel->set_t_on();
        newChannel->get_ops().push_back(client);
        initJOINReply(client,  newChannel);
        channel_msg(client, REPLY_JOIN(client->getNickname(), client->getUsername(), name, client->get_client_host()), name);
        }
    }
    }


}