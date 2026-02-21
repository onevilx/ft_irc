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

std::vector<Client *>& Channel::get_ops(){
    return this->operators;
}

void Channel::add_operator(Client *op){
    this->operators.push_back(op);
}

void Channel::set_limit(size_t limit){
    this->limit = limit;
}

size_t Channel::get_limit(){
    return this->limit;
}


std::string Channel::get_channel_mode()
{
	std::string mode_rtn = "+";
	if (this->get_i() == true)
		mode_rtn += "i";
	if (this->get_k() == true)
		mode_rtn += "k";
	if (this->get_l() == true)
		mode_rtn += "l";
	if (this->get_t() == true)
		mode_rtn += "t";
	return (mode_rtn);
}

bool channel_nrestriction(std::string str){
    for (int i; i < str.length(); i++)
        if(str[i] == ':' || str[i] == 7)
            return false; // here i will see if i would prefer to throw an error
    return true;
}

Client * Channel::get_client(char *str){
    for(std::vector<Client *>::iterator itr = this->get_ClientsinChannel().begin(); itr != this->get_ClientsinChannel().end(); itr++)
        if((*itr)->getNickname() == str)
            return (*itr);
}

bool is_client_in_channel1(std::vector<Client *> Cleints, int fd){
    for (size_t i = 0; i < Cleints.size(); i++)
        if(Cleints[i]->getFd() == fd)
            return true;
    return false;
}

bool Channel::addtoChannel(Client *client, std::string key){
    if(is_client_in_channel1(Clients, client->getFd()))
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
std::string Channel::get_key_val(){
    return this->key;
}

std::string Channel::get_Cname(){
    return this->Cname;
}

std::string Channel::getCreationTime(){
    return this->channelTime;
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
void Channel::setTheChannelTimeCreated()
{
    time_t present_time;
    present_time = time(NULL);
    std::ostringstream oss;
    oss << present_time;
    std::string time_now = oss.str();
    this->channelTime = time_now;
}
Channel::Channel(Server *server,std::string name, std::string pas): _server(server), Cname(name), key(pas){
    init_modes();
    setTheChannelTimeCreated();
}
Channel::Channel(const Channel& copy){

}
Channel::~Channel(){

} 

size_t Channel::count_users(){
    size_t i = 0;
    for (std::vector<Client *>::iterator it = this->Clients.begin(); it != this->Clients.end(); it++)
        i++;
    return i;
}

    void Channel::set_i_on(){
        this->i = true;
    }
    void Channel::set_k_on(){
    this->k = true;
    }
    void Channel::set_k(std::string Val){
        this->key = Val;
    }
    void Channel::set_l_on(){
        this->l = true;
    }
    void Channel::set_t_on(){
        this->t = true;
    }

        void Channel::set_i_off(){
            this->i = false;
        }
    void Channel::set_k_off(){
        this->k = false;
    }
    void Channel::set_l_off(){
        this->l = false;
    }
    void Channel::set_t_off(){
        this->t = false;
    }