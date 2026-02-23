#include "../headers/channel.hpp"
#include "../headers/client.hpp"
#include "../headers/server.hpp"
#include "../headers/replies.hpp"


void Server::kick(Client *user, Commands cmd){
    std::vector<std::string> args = cmd.getArgs();

    if(args.empty() && args.size() < 1 )
    {
       // 461
       std::string str = ERROR_NEEDMOREPARAMS(user->getNickname(), user->getHostname());
       send(user->getFd(), str.c_str(), str.length(), 0);
       return; 
    }
    Channel *cl = get_single_channel(args[0]);
    if(!cl){
        // 403
        std::string str = ERROR_NOSUCHCHANNEL(user->getHostname(), args[0], user->getNickname());
        send(user->getFd(), str.c_str(), str.length(), 0);
        return ;
    }
    Client *target = this->findClientByNick(args[1]);
    if(!target){
        // 401
        std::string str = ERROR_NOSUCHNICK(user->getHostname(), args[0], args[1]);
        send(user->getFd(), str.c_str(), str.length(), 0);
        return ;
    }
    if(cl->is_operator(user)){
        
        if(!cl->is_client_in_channel(target)){
            // 441
            std::string str = ERROR_USERNOTINCHANNEL(user->getHostname(), cl->get_Cname());
            send(user->getFd(), str.c_str(), str.length(), 0);
            return;
        }
            std::cout << "gere" << std::endl;
        for (std::vector<Client *>::iterator itr = cl->get_ClientsinChannel().begin(); itr != cl->get_ClientsinChannel().end(); itr++)
        {
            if((*itr)->getNickname() == target->getNickname()){
                cl->get_ClientsinChannel().erase(itr);
                break;
            }  
        }
            std::cout << "gere" << std::endl;
        for(std::vector<Client *>::iterator itr = cl->get_ops().begin(); itr != cl->get_ops().end(); itr++){
            if((*itr)->getNickname() == target->getNickname()){
                cl->get_ops().erase(itr);
                break;
            }   
        }
        for(std::vector<Client *>::iterator itr = cl->get_inv().begin(); itr != cl->get_inv().end(); itr++){
            if((*itr)->getNickname() == target->getNickname()){
                cl->get_inv().erase(itr);
                break;
            }
        }
        std::string msg = REPLY_KICK(user->getNickname(), user->getUsername(), user->getHostname(), cl->get_Cname(), target->getUsername(), "no comment is given");
        cl->send_toclients(msg);
        send(target->getFd(), msg.c_str(), msg.length(), 0);
    }
    else{
        std::string str = ERROR_NOPRIVILEGES(user->getHostname(), cl->get_Cname());
        send(user->getFd(), str.c_str(), str.length(), 0);
        return;
    }
}