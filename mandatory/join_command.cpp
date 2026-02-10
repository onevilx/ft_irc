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

void Server::join(Client *client, Commands cmd){
    std::vector<std::string> args = cmd.getArgs();

    std::stringstream ss(args[0]);
    std::string n;
    ss >> n;

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
    return;
   }
   // 
   join :#2,#2

}