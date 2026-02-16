#include "../headers/server.hpp"
#include "../headers/replies.hpp"

void Server::handleTopic(Client* client, Commands& cmd)
{
    const std::vector<std::string>& args = cmd.getArgs();

    if (args.empty())
    {
        std::string err = ERROR_NEEDMOREPARAMS(client->getNickname(), "TOPIC");
        send(client->getFd(), err.c_str(), err.size(), 0);
        return;
    }

    std::string channelName = args[0];

    if (channelName.empty() || (channelName[0] != '#' && channelName[0] != '&'))
    {
        std::string err = ERROR_NOSUCHCHANNEL(
            client->getHostname(),
            channelName,
            client->getNickname()
        );
        send(client->getFd(), err.c_str(), err.size(), 0);
        return;
    }

    Channel* channel = findChannel(channelName);

    if (!channel)
    {
        std::string err = ERROR_NOSUCHCHANNEL(
            client->getHostname(),
            channelName,
            client->getNickname()
        );
        send(client->getFd(), err.c_str(), err.size(), 0);
        return;
    }

    if (!channel->isMember(client))
    {
        std::string err = ERR_NOTONCHANNEL(
            client->getHostname(),
            channelName,
            client->getNickname()
        );
        send(client->getFd(), err.c_str(), err.size(), 0);
        return;
    }

    // SHOW TOPIC
    if (args.size() == 1)
    {
        if (channel->get_topic().empty())
        {
            std::string reply = RPL_NOTOPIC(client->getHostname(), channelName);
            send(client->getFd(), reply.c_str(), reply.size(), 0);
        }
        else
        {
            std::string reply = RPL_TOPIC(
                client->getHostname(),
                channelName,
                channel->get_topic()
            );
            send(client->getFd(), reply.c_str(), reply.size(), 0);

            std::string whoTime = REPLY_TOPICWHOTIME(
                channel->get_topic_setter(),
                channel->get_topic_time(),
                client->getNickname(),
                client->getHostname(),
                channelName
            );
            send(client->getFd(), whoTime.c_str(), whoTime.size(), 0);
        }
        return;
    }

    // SET TOPIC
    std::string newTopic = stripTrailingColon(args[1]);

    if (channel->get_t() && !channel->isOperator(client))
    {
        std::string err = ERROR_NOPRIVILEGES(
            client->getHostname(),
            channelName
        );
        send(client->getFd(), err.c_str(), err.size(), 0);
        return;
    }

    channel->set_topic(newTopic);
    channel->set_topic_setter(client->getNickname());

    time_t now = time(NULL);
    channel->set_topic_time(std::to_string(now));

    std::string msg = ":" +
        client->getNickname() + "!" +
        client->getUsername() + "@" +
        client->getHostname() +
        " TOPIC " + channelName +
        " :" + newTopic + "\r\n";

    channel->broadcast(msg);
}
