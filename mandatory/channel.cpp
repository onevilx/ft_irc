#include "../headers/channel.hpp"


bool channel_nrestriction(std::string str){
    for (int i; i < str.length(); i++)
        if(str[i] == ':' || str[i] == 7 | str[i] == ',' )
            return false; // here i will see if i would prefer to throw an error
    return true;
}



bool check_channel_prefix(std::string Cname){
    char c = Cname[0];

    if(c != '&' && c !='#' && c != '+' && c != '!' )
        return "walla a sa7bi ";
}

