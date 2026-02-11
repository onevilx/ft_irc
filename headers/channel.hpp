#ifndef Channel_HPP
#define Channel_HPP


#include "client.hpp"
#include <vector>
// here ineed a client class 
class Client;

class Channel
{
    private:
    std::string Cname; // there are some conditions to respect in the naming phase
    std::vector<Client *>  Clients;// this would be the users insde a channel
    // here i will figure if i need to store moderstors in a single vector or seek another apraoch 
    // we'll figure it later
    std::string topic;
    std::string topic_setter;
    std::string topic_time;
    std::string channelTime; //to store the time which the channel got created 

    int moderators; // here i will track the number of moderator in oe channel
    

    //channel features
    bool i; // inv 
    bool k; //
    std::string key;
    bool l;// limit a jmmi
    bool t;//topic restrict

    size_t limit; // this will be trigred if the l is setted as true 

    public:
    // canonical form 
    Channel();
    Channel(std::string name, std::string key);
    Channel(const Channel& copy);
    ~Channel();

    //setters to true 
    void set_i_on();
    void set_k_on();
    void set_k(std::string Val);
    void set_l_on();
    void set_t_on();

    void set_i_off();
    void set_k_off();
    void set_l_off();
    void set_t_off();
    

    // normal setters
    std::string set_Cname();
    std::string set_topic(std::string topic);
    std::string set_topic_setter();
    std::string set_topic_time();
    std::string set_Ctime();

    // getters
    bool get_k();
    bool get_i();
    bool get_l();
    bool get_t();
    std::string get_Cname();
    std::string get_topic();
    std::string get_topic_setter();
    std::string get_topic_time();
    std::string get_Ctime();

    bool addtoChannel(Client *Client, std::string key);
    
    


};



#endif
