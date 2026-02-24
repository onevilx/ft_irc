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
   for (size_t i = 0; i < this->channels.size(); i++)
        if(to_fetch == this->channels[i]->get_Cname())
            return (this->channels[i]);

    return NULL;
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

void free_mode_list(mode_arg *head)
{
    while (head)
    {
        mode_arg *next = head->next;

        if (head->command)
            delete[] head->command;

        if (head->args)
        {
            for (int i = 0; head->args[i]; ++i)
                delete[] head->args[i];

            delete[] head->args;
        }

        delete head;
        head = next;
    }
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

bool is_number(std::string str)
{
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

bool Channel::is_client_in_channel(Client * user){
    for(std::vector<Client *>::iterator itr = this->Clients.begin(); itr != this->Clients.end(); itr++)
        if((*itr)->getNickname() == user->getNickname())
            return true;
    return false;
}

bool Channel::is_operator(Client * user){
    for(std::vector<Client *>::iterator itr = this->get_ops().begin(); itr != this->get_ops().end(); itr++)
        if((*itr)->getNickname() == user->getNickname())
            return true;
    return false;
}

bool is_clientinchannel(char *str, Channel *cl){
    for (std::vector<Client *>::iterator itr = cl->get_ClientsinChannel().begin(); itr != cl->get_ClientsinChannel().end(); itr++)
        if((*itr)->getNickname() == str)
            return true;
    return false;
}


void Channel::apply_mode(Client *user,char c, char **str, int indx, bool tr){
    if(is_operator(user)){
        if(tr){
         if(c == 'l'){
        if(str[indx]){
        std::string rep_mo = "+l";
        if(is_number(str[indx])){
            if((long)this->get_limit() != atol(str[indx])){
        rep_mo += " ";
        rep_mo.append(str[indx]);
        this->set_l_on();
        this->set_limit(atol(str[indx]));//i need to parse the limit number
        std::string msg = REPLY_CHANNELMODES__(user->getUsername(), this->get_Cname(), user->getNickname(),rep_mo);
        send_toclients(msg);
        }
        }
        else
            return;
    }
    else{

        std::string msg = ERROR_INVALIDMODEPARAM_LIMIT(this->get_Cname(), user->getHostname(), "l");
        send(user->getFd(), msg.c_str(), msg.length(), 0);
    }
    }
    else if (c == 'o'){
        for (std::vector<Client *>::iterator itr = this->get_ops().begin(); itr != this->get_ops().end(); itr++)
        {
            if((*itr)->getNickname() == user->getNickname())
                {
                    if(str[indx]){
                        if(is_client_in_channel(user)){
                            if(is_operator(user))
                            {
                                Client *target = _server->findClientByNick(str[indx]);
                                if(!target)
                                {
                                    std::string msg = ERROR_NOSUCHNICK(user->getHostname(), this->get_Cname(), str[indx]);
                                    send(user->getFd(), msg.c_str(), msg.length(), 0);
                                    return ;
                                }
                                if(is_clientinchannel(str[indx], this)){
                                for(std::vector<Client *>::iterator itr = this->get_ClientsinChannel().begin(); itr != this->get_ClientsinChannel().end(); itr++){
                                        if((*itr)->getNickname() == str[indx])
                                        {
                                            this->operators.push_back((*itr));
                                            std::string msg = REPLY_CHANNELMODES__(user->getUsername(), this->get_Cname(), user->getNickname(), "+o");
                                            send_toclients(msg);
                                            return;

                                        }
                                }
                                }
                                else{
                                     std::string msg =ERROR_USERNOTINCHANNEL(user->getHostname(), this->get_Cname());
                                send(user->getFd(), msg.c_str(), msg.length(), 0);

                                }
                            }
                            else{
                                std::string msg = ERROR_NOPRIVILEGES__(user->getHostname(), this->get_Cname(), str[indx]);
                                send(user->getFd(), msg.c_str(), msg.length(), 0);
                            }

                        }
                        else {
                            std::string msg = ERROR_NOTONCHANNEL(user->getHostname(), this->get_Cname());
                            send(user->getFd(), msg.c_str(), msg.length(), 0);
                            }

                    }
                    else{
                        std::string msg = ERROR_INVALIDMODEPARAM(this->get_Cname(), user->getHostname(), "o");
                        send(user->getFd(), msg.c_str(), msg.length(), 0);
                    }
                }
        }
    }  
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
                std::string msg = ERROR_INVALIDMODEPARAM__KEY(this->get_Cname(),user->getHostname(), "k");
                send(user->getFd(), msg.c_str(), msg.length(), 0);
            return ;
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
     if(c == 'l'){
        if(!this->get_l())
            return;
        std::string rep = "-l";
        this->set_l_off();
        std::string msg = REPLY_CHANNELMODES__(user->getUsername(), this->get_Cname(), user->getNickname(), rep);
        send_toclients(msg);
        }
        else if (c == 'o'){
        for (std::vector<Client *>::iterator itr = this->get_ops().begin(); itr != this->get_ops().end(); itr++)
        {
            if((*itr)->getNickname() == user->getNickname())
                {
                    if(str[indx]){
                        if(is_client_in_channel(user)){
                            if(is_operator(user))
                            {
                                Client *target = _server->findClientByNick(str[indx]);
                                if(!target)
                                {
                                    std::string msg = ERROR_NOSUCHNICK(user->getHostname(), this->get_Cname(), str[indx]);
                                    send(user->getFd(), msg.c_str(), msg.length(), 0);
                                    return ;
                                }
                                
                                if(is_clientinchannel(str[indx], this)){
                                    Client *us = get_client(str[indx]);
                                    if(!us){
                                        return ;
                                    }
                                    if(!is_operator(us))
                                        return ;
                                    for(std::vector<Client *>::iterator itr = this->get_ops().begin(); itr != this->get_ops().end(); itr++){
                                        if((*itr)->getNickname() == str[indx])
                                        {
                                            this->get_ops().erase(itr);
                                            std::string msg = REPLY_CHANNELMODES__(user->getUsername(), this->get_Cname(), user->getNickname(), "-o");
                                            send_toclients(msg);
                                            return;
                                        }
                                    }
                                    }
                                    else{
                                        std::string msg =ERROR_USERNOTINCHANNEL(user->getHostname(), this->get_Cname());
                                        send(user->getFd(), msg.c_str(), msg.length(), 0);

                                }
                            }
                            else{
                                std::string msg = ERROR_NOPRIVILEGES__(user->getHostname(), this->get_Cname(), str[indx]);
                                send(user->getFd(), msg.c_str(), msg.length(), 0);
                            }

                        }
                        else {
                            std::string msg = ERROR_NOTONCHANNEL(user->getHostname(), this->get_Cname());
                            send(user->getFd(), msg.c_str(), msg.length(), 0);
                            }

                    }
                    else{
                        std::string msg = ERROR_INVALIDMODEPARAM(this->get_Cname(), user->getHostname(), "o");
                        send(user->getFd(), msg.c_str(), msg.length(), 0);
                    }
                }
        }
    }  
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
            std::string msg = REPLY_CHANNELMODES__(user->getUsername(), this->get_Cname(), user->getNickname(), "-t");
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
else{
    std::string msg = ERROR_NOPRIVILEGES(user->getHostname(), this->get_Cname());
    send(user->getFd(), msg.c_str(), msg.length(), 0);
}
}


void shift_args(char **args)
{
    if (!args)
        return;

    int i = 0;

    while (args[i])
    {
        args[i] = args[i + 1];
        i++;
    }
}

void Server::mode(Client *client, Commands cmd){
    std::vector<Channel *> channls = this->get_channels();
    std::vector<std::string> args = cmd.getArgs();
    if(args.size() < 1){
        std::string msg = ERROR_NEEDMOREPARAMS(client->getNickname(), client->getHostname());
        send(client->getFd(), msg.c_str(), msg.length(), 0);
    }
    if(args.size() == 1){
        if(exists(args[0])){
            Channel *cl = get_single_channel(args[0]);
            if(!cl)
                return;
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
        int i = 0;
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
        mode_arg *list =  parse_modes(args);
        mode_arg *tmp = list;
      while (tmp)
      {
        bool tr;
       if(tmp->command[0] == '+')
            tr = true;
       else if (tmp->command[0] == '-')
            tr = false;
       for (size_t i = 1; tmp->command[i]; i++)
       {
        int arg_c = 1;
        if(tmp->command[i] != 'i' && tmp->command[i] != 'k' && tmp->command[i] != 't' && tmp->command[i] != 'o' && tmp->command[i] != 'l')
          {
              std::string msg = ERR_UNKNOWNMODE(client->getNickname(), client->getHostname(), cl->get_Cname(), tmp->command[i]);
              send(client->getFd(), msg.c_str(), msg.length(), 0);
          }
        else if (cl->is_client_in_channel(client)){
            if(tmp->args[0] == NULL)
                cl->apply_mode(client,tmp->command[i], tmp->args, 0 , tr);
            else {
                cl->apply_mode(client,tmp->command[i], tmp->args, arg_c - 1 , tr);
                shift_args(tmp->args);
                arg_c ++;
            }
        }
        else{
            std::string msg = ERROR_NOTONCHANNEL(client->getHostname(), cl->get_Cname());
            send(client->getFd(), msg.c_str(), msg.length(), 0);
        }
       }
       tmp = tmp->next;
      }
      free_mode_list(list);
    }
}
}