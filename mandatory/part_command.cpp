#include "../headers/channel.hpp"
#include "../headers/client.hpp"
#include "../headers/server.hpp"
#include "../headers/replies.hpp"


void Server::cleanup_empty_channels()
{
    for (std::vector<Channel*>::iterator it = this->channels.begin();
         it != channels.end(); )
    {
        if ((*it)->get_ClientsinChannel().empty())
        {
            delete *it;
            it = channels.erase(it);
        }
        else
            ++it;
    }
}

void Server::part(Client *client, Commands cmd){
    std::vector<std::string> args = cmd.getArgs();

    if(args.empty())
    {
        std::string  msg  = ERROR_NEEDMOREPARAMS(client->getNickname(), client->getHostname());
        send(client->getFd(), msg.c_str(), msg.length(), 0);
        return;
    }
    std::vector<std::string> names = split(args[0], ",");
    for(std::vector<std::string>::iterator itr = names.begin(); itr != names.end(); itr++){
        if(exists(*itr)){
            Channel *cl = get_single_channel((*itr));
            if(cl->is_client_in_channel(client)){
                for(std::vector<Client *>::iterator ind = cl->get_ClientsinChannel().begin(); ind != cl->get_ClientsinChannel().end(); ind++)
                {
                    if((*ind)->getNickname() == client->getNickname()){
                        cl->get_ClientsinChannel().erase(ind);
                        std::string msg = PART_REPLY(client->getNickname(), client->getUsername(), client->getHostname(), cl->get_Cname(), "");
                        send(client->getFd(), msg.c_str(), msg.length(), 0);
                        cl->send_toclients(msg);
                        break;
                    }
                }
                for (std::vector<Client *>::iterator itr2 = (cl)->get_ops().begin(); itr2 != (cl)->get_ops().end(); itr2++)
                {
                if((*itr2)->getNickname() == client->getNickname()){
                    (cl)->get_ops().erase(itr2);
                    break;
                }
                }
                for (std::vector<Client *>::iterator itr2 = (cl)->get_inv().begin(); itr2 != (cl)->get_inv().end(); itr2++)
                {
                if((*itr2)->getNickname() == client->getNickname()){
                    (cl)->get_inv().erase(itr2);
                    break;
                }
                }
                
            }
            
        }
        else{
            // 403
            std::string  msg  = ERROR_NOSUCHCHANNEL(client->getHostname(), *itr, client->getNickname());
            send(client->getFd(), msg.c_str(), msg.length(), 0);
        }
    }

}