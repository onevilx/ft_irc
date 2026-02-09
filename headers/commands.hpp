#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

class Commands
{
private:
    std::string                 _command;
    std::vector<std::string>    _args;

    void parse(const std::string& rawLine);

public:
    Commands();
    explicit Commands(const std::string& rawLine);

    const std::string&              getCommand() const;
    const std::vector<std::string>& getArgs() const;
};

#endif
