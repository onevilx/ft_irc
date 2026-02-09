#include "../headers/commands.hpp"

Commands::Commands() {}

Commands::Commands(const std::string& rawLine)
{
    parse(rawLine);
}

void Commands::parse(const std::string& rawLine)
{
    std::string line = rawLine;
    if (!line.empty() && line.back() == '\r')
        line.pop_back();

    std::istringstream iss(line);

    if (!(iss >> _command))
        return;

    std::transform(_command.begin(), _command.end(), _command.begin(), ::toupper);

    std::string token;
    bool trailing = false;
    std::string trailingStr;

    while (iss >> token)
    {
        if (!trailing && token[0] == ':')
        {
            trailing = true;
            trailingStr = token.substr(1);
        }
        else if (trailing)
        {
            trailingStr += " " + token;
        }
        else
        {
            _args.push_back(token);
        }
    }

    if (trailing)
        _args.push_back(trailingStr);
}

const std::string& Commands::getCommand() const
{
    return _command;
}

const std::vector<std::string>& Commands::getArgs() const
{
    return _args;
}
