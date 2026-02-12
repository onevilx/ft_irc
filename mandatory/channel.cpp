#include "../headers/channel.hpp"

    bool Channel::get_k(){
        return k;
    }
    bool Channel::get_i(){
        return i;
    }
    bool Channel::get_l(){
        return l;
    }
    bool Channel::get_t(){
        return t;
    }

std::vector<Client *>& Channel::get_ClientsinChannel(){
    return this->Clients;
}

bool channel_nrestriction(std::string str){
    for (int i; i < str.length(); i++)
        if(str[i] == ':' || str[i] == 7)
            return false; // here i will see if i would prefer to throw an error
    return true;
}

bool is_client_in_channel(std::vector<Client *> Cleints, int fd){
    for (size_t i = 0; i < Cleints.size(); i++)
        if(Cleints[i]->getFd() == fd)
            return true;
    return false;
}

bool Channel::addtoChannel(Client *client, std::string key){
    if(is_client_in_channel(Clients, client->getFd()))
        return false;
    if(get_k() == true){
        // later
        std::cout << "key required" << std::endl;
    }
    bool shoulbeop = Clients.empty();
    // here i need to set it as operator if the house is empty
    client->set_operator(shoulbeop);
    Client *newClient = new Client(*client);
    Clients.push_back(newClient);
    return true;
}


bool check_channel_prefix(std::string Cname){
    char c = Cname[0];

    if(c != '&' && c !='#' && c != '+' && c != '!' )
        return "walla a sa7bi ";
    
}

std::string Channel::get_Cname(){
    return this->Cname;
}

void Channel::init_modes(){
    this->l = false;
    this->i = false;
    this->t = false;
}

Channel::Channel(){
    this->key = "";
    init_modes();
}
Channel::Channel(std::string name, std::string pas): Cname(name), key(pas){
    init_modes();
}
Channel::Channel(const Channel& copy){

}
Channel::~Channel(){

} 