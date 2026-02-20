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
    std::cout << "here normal" << std::endl;
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

bool is_number(std::string str)
{
    std::cout << "here2" << std::endl;
	if (str[0] == '+')
		str = str.substr(1, str.size());
	size_t found = str.find_first_not_of("0123456789");
	if (found == std::string::npos && \
		std::atol(str.c_str()) <= 9223372036854775807)
		return (true);
	return (false);
}

 void Channel::send_toclients(std::string msg){
    for (std::vector<Client *>::iterator itr = this->get_ClientsinChannel().begin(); itr != this->get_ClientsinChannel().end(); itr++){
        send((*itr)->getFd(), msg.c_str(), msg.length(), 0);
    }
}


void Channel::apply_mode(Client *user,char c, char **str, int indx, bool tr){
    std::cout << "in apply " << std::endl;
    if(tr){
        std::cout << "+ block" << std::endl;
    if(c == 'l'){
        if(str[indx]){
        std::string rep_mo = "+l";
        if(is_number(str[indx])){
            if(this->get_limit() != atol(str[indx])){
        rep_mo += " ";
        rep_mo.append(str[indx]);
        this->set_l_on();
        this->set_limit(atol(str[indx]));//i need to parse the limit number
        std::string msg = REPLY_CHANNELMODES__(user->getUsername(), this->get_Cname(), user->getNickname(),rep_mo);
        send_toclients(msg);
        }
        }
    }
    else{

        std::string msg = ERROR_INVALIDMODEPARAM_LIMIT(this->get_Cname(), user->getHostname(), "l");
        send(user->getFd(), msg.c_str(), msg.length(), 0);
    }
    }
    // else if (c == 'o'){
        

    // }
    else if(c == 'k'){
        if(!this->get_k() && str[indx]){
            this->set_k_on();
            std::string rep_mo = "+k";
            rep_mo += " ";
            rep_mo.append(str[indx]);
            this->set_k(str[indx]);
            std::string msg = REPLY_CHANNELMODES__(user->getUsername(), this->get_Cname(), user->getNickname(), rep_mo);
            send_toclients(msg);
        }
        else{
            return ;
                std::string msg = ERROR_INVALIDMODEPARAM__KEY(this->get_Cname(),user->getHostname(), "k");
                send(user->getFd(), msg.c_str(), msg.length(), 0);
            }
    }
    else if(c == 't'){
        if(!this->get_t()){
            this->set_t_on();
            std::string rep_mo = "+t";
            std::string msg = REPLY_CHANNELMODES__(user->getUsername(), this->get_Cname(), user->getNickname(), rep_mo);
            send_toclients(msg);
        }
    }
    else if(c == 'i'){
        if(!this->get_i()){
            this->set_i_on();
            std::string rep_mo = "+i";
            std::string msg = REPLY_CHANNELMODES__(user->getUsername(), this->get_Cname(), user->getNickname(), rep_mo);
            send_toclients(msg);
        }
    }
}
else if (!tr){
    std::cout << "- block " << std::endl;
     if(c == 'l'){
        if(!this->get_l())
            return;
        std::string rep = "-l";
        this->set_l_off();
        std::string msg = REPLY_CHANNELMODES__(user->getUsername(), this->get_Cname(), user->getNickname(), rep);
        send_toclients(msg);
        }
    // else if (c == 'o'){
        

    // }
    else if(c == 'k'){
        if(str[indx]){
            if(!this->get_k()){
                return;
            }
            if(this->get_key_val() == str[indx]){
            std::string rep_mo  = "-k *";
            this->set_k_off();
            this->key = "";
            std::string msg = REPLY_CHANNELMODES__(user->getUsername(), this->get_Cname(), user->getNickname(), rep_mo);
            send_toclients(msg);
            }
            else if(this->get_key_val() != str[indx] ){
                std::string msg = ERR_KEYSET(user->getHostname(), this->get_Cname());
                send(user->getFd(), msg.c_str(), msg.length(), 0);
            }
            else{
                return ;
            }

        }
        else{
                std::string msg = ERROR_INVALIDMODEPARAM__KEY(this->get_Cname(),user->getHostname(), "k");
                send(user->getFd(), msg.c_str(), msg.length(), 0);
            }
    }
    else if(c == 't'){
        if(this->get_t()){
            this->set_t_off();
            std::string msg = REPLY_CHANNELMODES__(user->getUsername(), this->get_Cname(), user->getNickname(), this->get_channel_mode());
            send_toclients(msg);
        }
    }
    else if(c == 'i'){
        if(this->get_i()){
           this->set_i_off();
           std::string rep = "-i";
           std::string msg = REPLY_CHANNELMODES__(user->getUsername(), this->get_Cname(), user->getNickname(), rep);
           send_toclients(msg);
        }

    }
}

}


void Server::mode(Client *client, Commands cmd){
    std::vector<Channel *> channls = this->get_channels();
    std::vector<std::string> args = cmd.getArgs();
   std::cout << "here 1"  << std::endl;
    if(args.size() < 1){
        std::cout << "here 2"  << std::endl;
        std::string msg = ERROR_NEEDMOREPARAMS(client->getNickname(), client->getHostname());
        send(client->getFd(), msg.c_str(), msg.length(), 0);
    }
    if(args.size() == 1){
        std::cout << "here 3"  << std::endl;
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
        std::cout << "here 4"  << std::endl;
        if(exists(args[0])){
        Channel *cl = get_single_channel(args[0]);
        args.erase(args.begin());
        int i = 0;
        // std::cout << args[0] <<  "-------------------------------------------------------------------------"<< std::endl;
        while(args[0][i] && args[0][i] != '-' && args[0][i] != '+')
        {
            if(args[0][i] != 'i' && args[0][i] != 'k' && args[0][i] != 't' && args[0][i] != 'o' && args[0][i] != 'l')
            {
              std::string msg = ERR_UNKNOWNMODE(client->getNickname(), client->getHostname(), cl->get_Cname(), args[0][i]);
              send(client->getFd(), msg.c_str(), msg.length(), 0);
            }
          i++;
        }
        args[0] = args[0].substr(i);
        if(args[0].empty())
            return;
        args[0] = normalize_modes(args[0]);
        std::cout << args[0] << std::endl;
        mode_arg *list =  parse_modes(args);
        std::cout << list->command << std::endl;
        
      // here i wiil inject my logic that gets command sequence like this -lk+oi -lk +o


      while (list)
      {
        std::cout << "here 5"  << std::endl;
        bool tr;
        std::cout << list->command << std::endl;
        std::cout << "insideiloop" << std::endl;
       if(list->command[0] == '+')
            tr = true;
       else if (list->command[0] == '-')
            tr = false;
       for (size_t i = 1; list->command[i]; i++)
       {
        if(list->command[i] != 'i' && list->command[i] != 'k' && list->command[i] != 't' && list->command[i] != 'o' && list->command[i] != 'l')
          {
              std::string msg = ERR_UNKNOWNMODE(client->getNickname(), client->getHostname(), cl->get_Cname(), list->command[i]);
              send(client->getFd(), msg.c_str(), msg.length(), 0);
          }
        else{
            std::cout << "apply block" << std::endl;
          cl->apply_mode(client,list->command[i], list->args, i - 1 , tr); // here i need to pass the index as the - or + case
        }
       }
       
       list = list->next;
      }
    }
}
}