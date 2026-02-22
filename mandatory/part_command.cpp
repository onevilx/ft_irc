#include "../headers/channel.hpp"
#include "../headers/client.hpp"
#include "../headers/server.hpp"
#include "../headers/replies.hpp"


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
                        // here i need to handle operator
                        delete_channel_if_no_still(cl);
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