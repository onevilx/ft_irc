#include "../headers/channel.hpp"
#include "../headers/client.hpp"
#include "../headers/server.hpp"
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

void Server::join(Client *client, Commands cmd){

    std::vector<std::string> args = cmd.getArgs();

    for (size_t i = 0; i < args.size(); i++)
    {
        std::cout << args[i] << "ff" << std::endl;
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
   if(args.size() < 1 || args[0].empty()){
    // rfc response
    std::cout << "need params err" << std::endl;
    return;
   }
   Cnames = split(args[0], ",");
   for (std::vector<std::string>::iterator it = Cnames.begin(); it != Cnames.end(); ++it){
    if(args.size() == 1){
        std::string name = args[0];
        if(name[0] != '#' || name.empty() || name.substr(1).find(" ") != std::string::npos)
            return ; // error to be aaded later;
        // here i wil check for the channel presence 
       if(exists(name)){
        // here i wont creat it so i will just add the user to it under the requirements to set later
       }
       else{
        std::cout << "feefefef" << std::endl;
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
        // initJOINReply(client, newChannel);
        // channel_msg(client, REPLY_JOIN(client->getNickname(), client->getUsername(),), name);
        // 
        }
    }
    }


}