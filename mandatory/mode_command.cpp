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


struct mode
{
    char *command;
    char **args;
    mode *next;
};


std::vector<Channel *>& Server::get_channels(){
    return this->channels;
}

Channel* Server::get_single_channel(std::string to_fetch){
   for (int i = 0; i < this->channels.size(); i++)
        if(to_fetch == this->channels[i]->get_Cname())
            return (this->channels[i]);
}



bool modeNeedsParam(char c)
{
    if (c == 'o' || c == 'l' || c == 'k')
        return true;
    return false;
}

char *strdup_cpp98(const std::string &s)
{
    char *dup = new char[s.size() + 1];
    std::strcpy(dup, s.c_str());
    return dup;
}

mode_arg *create_node(const std::string &cmd,
                  const std::vector<std::string> &arguments)
{
    mode_arg *node = new mode_arg;
    node->next = NULL;

    node->command = strdup_cpp98(cmd);

    node->args = new char *[arguments.size() + 1];
    for (size_t i = 0; i < arguments.size(); ++i)
        node->args[i] = strdup_cpp98(arguments[i]);

    node->args[arguments.size()] = NULL;

    return node;
}

mode_arg *parse_modes(std::vector<std::string> &input)
{
    if (input.empty())
        return NULL;

    std::string modes = input[0];
    size_t paramIndex = 1;

    mode_arg     *head = NULL;
    mode_arg     *tail = NULL;

    std::string currentCmd;
    std::vector<std::string> currentArgs;

    for (size_t i = 0; i < modes.size(); ++i)
    {
        if (modes[i] == '+' || modes[i] == '-')
        {
            if (!currentCmd.empty())
            {
                mode_arg *node = create_node(currentCmd, currentArgs);
                if (!head)
                    head = node;
                else
                    tail->next = node;
                tail = node;

                currentArgs.clear();
            }
            currentCmd = modes[i];
        }
        else
        {
            currentCmd += modes[i];

            if (modeNeedsParam(modes[i]))
            {
                if (paramIndex < input.size())
                {
                    currentArgs.push_back(input[paramIndex]);
                    paramIndex++;
                }
            }
        }
    }

    if (!currentCmd.empty())
    {
        mode_arg *node = create_node(currentCmd, currentArgs);
        if (!head)
            head = node;
        else
            tail->next = node;
    }

    return head;
}

std::string normalize_modes(const std::string &raw)
{
    std::string plusBlock;
    std::string minusBlock;
    char currentSign = 0;

    for (size_t i = 0; i < raw.size(); ++i)
    {
        if (raw[i] == '+' || raw[i] == '-')
        {
            currentSign = raw[i];
        }
        else
        {
            if (!currentSign)
                continue;

            if (currentSign == '+')
                plusBlock += raw[i];
            else if (currentSign == '-')
                minusBlock += raw[i];
        }
    }

    std::string result;

    if (!plusBlock.empty())
        result += "+" + plusBlock;

    if (!minusBlock.empty())
        result += "-" + minusBlock;

    return result;



}

void Channel::apply_mode(char c, char **str, int indx, bool tr){
    // std::string modes[5] = {"l", "o", "k", "t", "i"};
    if(1){
    if(c == 'l'){
        if(!this->get_l()){
            this->set_l_on();
         //   this->set_limit( ); i need to parse the limit number
            
        }

    }
    else if (c == 'o'){
        

    }
    else if(c == 'k'){
        if(this->get_k()){

        }

    }
    else if(c == 't'){

    }
    else if(c == 'i'){

    }
}
else{

}
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
        args.erase(args.begin());
        std::cout << args[0] <<  "-------------------------------------------------------------------------"<< std::endl;
        args[0] = normalize_modes(args[0]);
        std::cout << args[0] << std::endl;
        mode_arg *list =  parse_modes(args); 
        
      // here i wiil inject my logic that gets command sequence like this -lk+oi -lk +o


      while (list)
      {
        
        bool tr;
    //    std::string str = list->command;
       if(list->command[0] == '+')
            tr = true;
       else
            tr = false;
       for (size_t i = 0; list->command[i]; ++i)
       {
        if(list->command[i] != 'i' && list->command[i] != 'k' && list->command[i] != 't' && list->command[i] != 'o' && list->command[i] != 'l')
          {
              std::string msg = ERR_UNKNOWNMODE(client->getNickname(), client->getHostname(), cl->get_Cname(), list->command[i]);
              send(client->getFd(), msg.c_str(), msg.length(), 0);
          }
        else
          cl->apply_mode(list->command[i], list->args, i+1, tr); // here i need to pass the index as the - or + case

       }
       
       list = list->next;
      }
    
// while (tmp)
// {
//     std::cout << "------ NODE ------" << std::endl;

//     if (tmp->command)
//         std::cout << "command: " << tmp->command << std::endl;
//     else
//         std::cout << "command: NULL" << std::endl;

//     if (tmp->args)
//     {
//         int i = 0;
//         while (tmp->args[i])
//         {
//             std::cout << "arg[" << i << "] = "
//                       << tmp->args[i] << std::endl;
//             i++;
//         }
//     }
//     else
//     {
//         std::cout << "args: NULL" << std::endl;
//     }

//     tmp = tmp->next;
// }

        // 
        // if( != '+' && list->command[0] != '-'){
            // for (size_t i = 0; args[1][i]; i++)
            // {
                // if(args[1][i] != 'i' && args[1][i] != 'k' && args[1][i] != 't' && args[1][i] != 'o' && args[1][i] != 'l')
                // {
                    // std::string msg = ERR_UNKNOWNMODE(client->getNickname(), client->getHostname(), args[0], args[1][i]);
                    // send(client->getFd(), msg.c_str(), msg.length(), 0);
                // }
            // }
        // }
        // else if (args[1][0] == '+' || args[1][0] == '-'){
            //  for (size_t i = 1; args[1][i]; i++)
            // {
                // if(args[1][i] != 'i' && args[1][i] != 'k' && args[1][i] != 't' && args[1][i] != 'o' && args[1][i] != 'l')
                // {
                    // std::string msg = ERR_UNKNOWNMODE(client->getNickname(), client->getHostname(), args[0], args[1][i]);
                    // send(client->getFd(), msg.c_str(), msg.length(), 0);
                // }
                // else{
                    // here i need to add the modes
                    // if(args[1][i] != 'i'){
                        // if(!cl->get_i()){
                            // cl->set_l_on();
                            // std::string msg = REPLY_CHANNELMODES__(client->getUsername(), cl->get_Cname(), client->getNickname(), cl->get_channel_mode());
                            // send(client->getFd(), msg.c_str(), msg.length(), 0);
                        // }
                        // 
                    // }
                    // else if(args[1][i] != 'k'){
                            //  if(!cl->get_k()){
                            // cl->set_k_on();
                            // std::string msg = REPLY_CHANNELMODES__(client->getUsername(), cl->get_Cname(), client->getNickname(), cl->get_channel_mode());
                            // send(client->getFd(), msg.c_str(), msg.length(), 0);
                        // }
// 
                        // }
// 
                    // else if(args[1][i] != 't'){
                            //  if(!cl->get_t()){
                            // cl->set_t_on();
                            // std::string msg = REPLY_CHANNELMODES__(client->getUsername(), cl->get_Cname(), client->getNickname(), cl->get_channel_mode());
                            // send(client->getFd(), msg.c_str(), msg.length(), 0);
                        // }
// 
                        // }
// 
                    // else if(args[1][i] != 'l'){
                            //  if(!cl->get_l()){
                            // cl->set_l_on();
                            // std::string msg = REPLY_CHANNELMODES__(client->getUsername(), cl->get_Cname(), client->getNickname(), cl->get_channel_mode());
                            // send(client->getFd(), msg.c_str(), msg.length(), 0);
                        // }
// 
                        // }
// 
                        // if(args[1][i] != 'o'){
                            // if(!cl->get_o()){
                        //     cl->set_o_on();
                        //     std::string msg = REPLY_CHANNELMODES__(client->getUsername(), cl->get_Cname(), client->getNickname(), cl->get_channel_mode());
                        //     send(client->getFd(), msg.c_str(), msg.length(), 0);
                        // }

                        // }

                    
                // }
        // }
    // }
    return;
    }
}
}