#include "../headers/server.hpp"
#include "../headers/replies.hpp"

/*
MODE - Change the channel’s mode:
· i: Set/remove Invite-only channel
· t: Set/remove the restrictions of the TOPIC command to channel operators
· k: Set/remove the channel key (password)
· o: Give/take channel operator privilege
· l: Set/remove the user limit to channel

*/

/*
    so the logic wil be as this: 

    i will process the commands from the left to the right 


*/

std::vector<Channel *>& Server::get_channels(){
    return this->channels;
}

Channel* Server::get_single_channel(std::string to_fetch){
   for (int i = 0; i < this->channels.size(); i++)
        if(to_fetch == this->channels[i]->get_Cname())
            return (this->channels[i]);
}

bool    parse_limit(std::string str[i]){
    
}


void Server::mode(Client *client, Commands cmd){
    std::vector<Channel *> channls = this->get_channels();
    std::vector<std::string> args = cmd.getArgs();

    // for(std::vector<std::string>::iterator it = args.begin(); it != args.end(); it++){
    //     std::cout << (*it)[0] << std::endl;
    // }
    /*
    here i will detect the "+" or "-"
    split the modes into single chuncks

    */
    if(args.size() < 1){
        std::string msg = ERROR_NEEDMOREPARAMS(client->getNickname(), client->getHostname());
        send(client->getFd(), msg.c_str(), msg.length(), 0);
    }
    if(args.size() == 1){
        if(exists(args[0])){
            Channel *cl = get_single_channel(args[0]);
            std::string rp = REPLY_CHANNELMODES(client->getHostname(), cl->get_Cname(), client->getNickname(), cl->get_channel_mode());
            send(client->getFd(), rp.c_str(), rp.length(), 0);
            std::string rp1 = REPLY_CREATIONTIME(client->getHostname(), cl->get_Cname(), client->getNickname(), cl->getCreationTime());
            send(client->getFd(), rp1.c_str(), rp1.length(), 0);
            return ;
        }
        else{
            std::string pr2 = ERROR_NOSUCHCHANNEL(client->getHostname(),args[0], client->getNickname());
            send(client->getFd(), pr2.c_str(), pr2.length(), 0);
            return ;
        }
    }
    if(args.size() > 1){
        if(exists(args[0])){
        Channel *cl = get_single_channel(args[0]);
        if(args[1][0] != '+' && args[1][0] != '-'){
            for (size_t i = 0; args[1][i]; i++)
            {
                if(args[1][i] != 'i' && args[1][i] != 'k' && args[1][i] != 't' && args[1][i] != 'o' && args[1][i] != 'l')
                {
                    std::string msg = ERR_UNKNOWNMODE(client->getNickname(), client->getHostname(), args[0], args[1][i]);
                    send(client->getFd(), msg.c_str(), msg.length(), 0);
                }
            }
        }
        else if (args[1][0] == '+' || args[1][0] == '-'){
             for (size_t i = 1; args[1][i]; i++)
            {
                if(args[1][i] != 'i' && args[1][i] != 'k' && args[1][i] != 't' && args[1][i] != 'o' && args[1][i] != 'l')
                {
                    std::string msg = ERR_UNKNOWNMODE(client->getNickname(), client->getHostname(), args[0], args[1][i]);
                    send(client->getFd(), msg.c_str(), msg.length(), 0);
                }
                else{
                    // here i need to add the modes
                    if(args[1][i] != 'i'){
                        if(!cl->get_i()){
                            cl->set_l_on();
                            std::string msg = REPLY_CHANNELMODES__(client->getUsername(), cl->get_Cname(), client->getNickname(), cl->get_channel_mode());
                            send(client->getFd(), msg.c_str(), msg.length(), 0);
                        }
                        
                    }
                    else if(args[1][i] != 'k'){
                             if(!cl->get_k()){
                            cl->set_k_on();
                            std::string msg = REPLY_CHANNELMODES__(client->getUsername(), cl->get_Cname(), client->getNickname(), cl->get_channel_mode());
                            send(client->getFd(), msg.c_str(), msg.length(), 0);
                        }

                        }

                    else if(args[1][i] != 't'){
                             if(!cl->get_t()){
                            cl->set_t_on();
                            std::string msg = REPLY_CHANNELMODES__(client->getUsername(), cl->get_Cname(), client->getNickname(), cl->get_channel_mode());
                            send(client->getFd(), msg.c_str(), msg.length(), 0);
                        }

                        }

                    else if(args[1][i] != 'l'){
                             if(!cl->get_l()){
                            cl->set_l_on();
                            if(!parse_limit(args[i])){

                            }
                            std::string msg = REPLY_CHANNELMODES__(client->getUsername(), cl->get_Cname(), client->getNickname(), cl->get_channel_mode());
                            send(client->getFd(), msg.c_str(), msg.length(), 0);
                        }

                        }

                        // if(args[1][i] != 'o'){
                        //     if(!cl->get_o()){
                        //     cl->set_o_on();
                        //     std::string msg = REPLY_CHANNELMODES__(client->getUsername(), cl->get_Cname(), client->getNickname(), cl->get_channel_mode());
                        //     send(client->getFd(), msg.c_str(), msg.length(), 0);
                        // }

                        // }

                    
                }
        }
    }
    return;
    }
}
}