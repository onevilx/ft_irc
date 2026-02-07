#ifndef Channel_HPP
#define Channel_HPP


#include "client.hpp"
#include <vector>
// here ineed a client class 
class Client;

class channel
{
    std::string Cname; // there are some conditions to respect in the naming phase
    std::vector<Client *>  Clients;// this would be the users insde a channel
    // here i will figure if i need to store moderstors in a single vector or seek another apraoch 
    // we'll figure it later
    std::string topic;
    std::string topic_setter;
    std::string topic_time;
    std::string channelTime; //to store the time which the channel got created 

    int moderators; // here i will track the number of moderator in oe channel
    
};



#endif
