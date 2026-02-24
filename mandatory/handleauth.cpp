#include "../headers/server.hpp"
#include "../headers/replies.hpp"
#include "../headers/client.hpp"

void Server::handleAuth(Client* client, Commands& cmd)
{
    const std::string& c = cmd.getCommand();
    const std::vector<std::string>& a = cmd.getArgs();

    if (c == "PASS")
    {
        if (client->isPassOk())
            return;

        if (a.empty() || a[0] != _password)
        {
            std::string err = ERROR_PASSWDMISMATCH(
                client->getNickname(), client->getHostname());
            send(client->getFd(), err.c_str(), err.size(), 0);
            return;
        }

        client->setPassOk();
        return;
    }

    if (!client->isPassOk())
    {
        std::string err = ERROR_PASSWDMISMATCH(
            client->getNickname(), client->getHostname());
        send(client->getFd(), err.c_str(), err.size(), 0);
        return;
    }

    else if (c == "NICK")
    {
        if (a.empty())
            return;

        std::string requested = stripTrailingColon(a[0]);
        Client* existing = findClientByNick(requested);

        if (existing && existing != client)
        {
            std::string err = ERROR_NICKNAMEINUSE(client->getNickname(), client->getHostname());
            send(client->getFd(), err.c_str(), err.size(), 0);
        }
        else
            client->setNickname(requested);
    }
    else if (c == "USER")
    {
        if (a.size() < 4)
        {
            std::string msg = ERROR_NEEDMOREPARAMS(client->getNickname(), client->getHostname());
            send(client->getFd(), msg.c_str(), msg.size(), 0);
            return;
        }

        client->setUsername(a[0]);
        client->setServername("ft_irc");

        std::string realname = a[3];
        if (!realname.empty() && realname[0] == ':')
            realname = realname.substr(1);

        client->setRealname(realname);
    }

    client->tryAuthenticate();

    if (client->isAuthenticated())
    {
        std::string welcome = REPLY_WELCOME(client->getNickname(), client->get_client_host());
        send(client->getFd(), welcome.c_str(), welcome.size(), 0);

        std::string yourhost = REPLY_YOURHOST(client->getNickname(), client->get_client_host());
        send(client->getFd(), yourhost.c_str(), yourhost.size(), 0);

        std::string created = REPLY_CREATED(client->getNickname(), client->get_client_host(), _creationTime);
        send(client->getFd(), created.c_str(), created.size(), 0);
    }
}
