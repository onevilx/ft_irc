#include "../headers/server.hpp"
#include "../headers/replies.hpp"

void Server::handlePrivmsg(Client* client, Commands& cmd)
{
    const std::vector<std::string>& args = cmd.getArgs();

    if (args.empty())
    {
        std::string err = ERROR_NEEDMOREPARAMS(client->getNickname(), client->getHostname());
        send(client->getFd(), err.c_str(), err.size(), 0);
        return;
    }

    if (args.size() < 2)
    {
        std::string err = ERR_NOTEXTTOSEND(client->getHostname());
        send(client->getFd(), err.c_str(), err.size(), 0);
        return;
    }

    std::string targetNick = stripTrailingColon(args[0]);
    std::string message    = args[1];

    Client* target = findClientByNick(targetNick);

    if (!target)
    {
        std::string err = ERR_NOSUCHNICK(client->getHostname(), targetNick);
        send(client->getFd(), err.c_str(), err.size(), 0);
        return;
    }

    std::string msg = PRIVMSG_FORMAT(
        client->getNickname(),
        client->getUsername(),
        client->getHostname(),
        targetNick,
        message
    );
    send(target->getFd(), msg.c_str(), msg.size(), 0);
}
