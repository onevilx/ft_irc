#include <iostream>
#include <vector>
#include <cstring>

struct mode
{
    char *command;
    char **args;
    mode *next;
};

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

mode *create_node(const std::string &cmd,
                  const std::vector<std::string> &arguments)
{
    mode *node = new mode;
    node->next = NULL;

    node->command = strdup_cpp98(cmd);

    node->args = new char *[arguments.size() + 1];
    for (size_t i = 0; i < arguments.size(); ++i)
        node->args[i] = strdup_cpp98(arguments[i]);

    node->args[arguments.size()] = NULL;

    return node;
}

mode *parse_modes(std::vector<std::string> &input)
{
    if (input.empty())
        return NULL;

    std::string modes = input[0];
    size_t paramIndex = 1;

    mode *head = NULL;
    mode *tail = NULL;

    std::string currentCmd;
    std::vector<std::string> currentArgs;

    for (size_t i = 0; i < modes.size(); ++i)
    {
        if (modes[i] == '+' || modes[i] == '-')
        {
            if (!currentCmd.empty())
            {
                mode *node = create_node(currentCmd, currentArgs);
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
        mode *node = create_node(currentCmd, currentArgs);
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

int main()
{
    std::string test = "-+-+-+oi+-+-lk";
std::cout << normalize_modes(test) << std::endl; 
}




// this for not re typing the code all over again 

  if(str[1][i] != 'i' && str[1][i] != 'k' && str[1][i] != 't' && str[1][i] != 'o' && str[1][i] != 'l')
                {
                    std::string msg = ERR_UNKNOWNMODE(client->getNickname(), client->getHostname(), args[0], args[1][i]);
                    send(client->getFd(), msg.c_str(), msg.length(), 0);
                } 
