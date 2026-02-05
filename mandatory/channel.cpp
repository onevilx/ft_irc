#include "../headers/channel.hpp"



bool check_channel_prefix(std::string Cname){
    char c = Cname[0];

    if(c != '&' && c !='#' && c != '+' && c != '!' )
        return "walla a sa7bi ";
}

