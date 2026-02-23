#include "../headers/server.hpp"
#include "../headers/replies.hpp"

void Server::handlePrivmsg(Client* client, Commands& cmd)
{
    const std::vector<std::string>& args = cmd.getArgs();

    // PRIVMSG <target> :<message>

    if (args.empty())
    {
        std::string err = ERROR_NEEDMOREPARAMS(
            client->getNickname(),
            client->get_client_host()
        );
        send(client->getFd(), err.c_str(), err.size(), 0);
        return;
    }

    if (args.size() < 2)
    {
        std::string err = ERR_NOTEXTTOSEND(
            client->get_client_host()
        );
        send(client->getFd(), err.c_str(), err.size(), 0);
        return;
    }

    std::string targetName = stripTrailingColon(args[0]);
    std::string message    = args[1];

    // Build formatted message once
    std::string formattedMsg = PRIVMSG_FORMAT(
        client->getNickname(),
        client->getUsername(),
        client->get_client_host(),
        targetName,
        message
    );

    // =========================
    // 1️⃣ CHANNEL MESSAGE
    // =========================
    if (!targetName.empty() && targetName[0] == '#')
    {
        Channel* channel = findChannel(targetName);
        if (!channel)
        {
            std::string err = ERROR_NOSUCHCHANNEL(
                client->get_client_host(),
                targetName,
                client->getNickname()
            );
            send(client->getFd(), err.c_str(), err.size(), 0);
            return;
        }

        // Check if sender is in the channel
        if (!channel->isMember(client))
        {
            std::string err = ERROR_NOSUCHCHANNEL(
                client->get_client_host(),
                targetName,
                client->getNickname()
            );
            send(client->getFd(), err.c_str(), err.size(), 0);
            return;
        }

        // Send to all members except sender
        std::vector<Client*>& members = channel->get_ClientsinChannel();

        for (std::vector<Client*>::const_iterator it = members.begin(); it != members.end(); ++it)
        {
            if (*it != client)
                send((*it)->getFd(), message.c_str(), message.length(), 0);
        }
        return;
    }

    Client* targetClient = findClientByNick(targetName);
    if (!targetClient)
    {
        std::string err = ERR_NOSUCHNICK(
            client->get_client_host(),
            targetName
        );
        send(client->getFd(), err.c_str(), err.size(), 0);
        return;
    }

    send(targetClient->getFd(),
         formattedMsg.c_str(),
         formattedMsg.size(),
         0);
}