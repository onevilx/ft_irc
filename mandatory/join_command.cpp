#include "../headers/channel.hpp"
#include "../headers/client.hpp"
#include "../headers/server.hpp"
#include "../headers/replies.hpp"
// #include "../headers/commands.hpp"

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
            // so the logic here i will send the join message to any client
            // in the channel excepts the current joining client
            if((*it)->get_ClientsinChannel()[i]->getNickname()  != user->getNickname())
                    sendToclient((*it)->get_ClientsinChannel()[i]->getFd(), msg);// send the message
        }
    }
}

bool    Server::isClinetinChannel(Client *user, std::string name){
        for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); it++)
        {
            for (std::vector<Client*>::iterator iter = (*it)->get_ClientsinChannel().begin(); iter != (*it)->get_ClientsinChannel().end(); iter++)
            {
                if((*iter)->getFd() == user->getFd())
                    // // here the mesage of the clinet is already in the channel
                    // std::cout << "rah deja kayn had khona " << std::endl;
                    return true;
            }
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
    // rfc response
    std::string msg = ERROR_NEEDMOREPARAMS(client->getNickname(), client->getHostname());
    send(client->getFd(), msg.c_str(), msg.length(), 0);
    // std::cout << "need params err" << std::endl;
    return;
   }
    

    std::stringstream ss(args[0]);
    std::string n;
    ss >> n;
    std::vector<std::string> Cnames;


    /* 
    zero logic to inject later
    if(n == "0"){
        joinzero(client);
        return;
    }
    */
   // here the error handler
   Cnames = split(args[0], ",");
   for (std::vector<std::string>::iterator i = Cnames.begin() ; i != Cnames.end(); i++)
   {
    std::cout << (*i) << std::endl;
   }
   
   for (std::vector<std::string>::iterator it = Cnames.begin(); it != Cnames.end(); ++it){
    if(args.size() == 1){
        std::string name = *it;
        if(name[0] != '#' || name.empty() || name.substr(1).find(" ") != std::string::npos){
            // std::cout << "prefix error" << std::endl;
            std::string msg = ERROR_NOSUCHCHANNEL(client->getHostname(), name, client->getNickname());
            send(client->getFd(), msg.c_str(), msg.length(), 0);
            return ; // error to be aaded later;
        }
        // here i wil check for the channel presence 
       if(exists(name)){
            if(isClinetinChannel(client, name)){
                std::string msg = ERROR_USERONCHANNEL(client->getHostname(), name, client->getNickname());
                send(client->getFd(), msg.c_str(), msg.length(), 0);
                continue;
            }
            else{
                // here i will ad the client to the channel
                // so here i will add the client in the chanel_clinets container aftr that i will send the join message to every client except the current client
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
        
       }
       else{
        // here the meat
        // i will creat the channel and add the user to it 
        Channel* newChannel = new Channel(name, "");
        // here i will add the client to the channel
        if(!newChannel->addtoChannel(client, ""))
            continue;  
        channels.push_back(newChannel);
        std::cout << "--------------------channels--------------------" << std::endl;
        for (size_t i = 0; i < channels.size(); i++)
        {
            
            std::cout << channels[i]->get_Cname() << std::endl;
        }
        // replys basedon the rfc
        initJOINReply(client,  newChannel);
        channel_msg(client, REPLY_JOIN(client->getNickname(), client->getUsername(), name, client->getHostname()), name);
        // 
        }
    }
    }


}