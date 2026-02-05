#include "../headers/client.hpp"

Client::Client(int fd) : _fd(fd), _nickname(""), _username(""), _buffer(""), _authenticated(false) {}

Client::~Client()
{
    close(_fd);
}

int Client::getFd() const
{
    return _fd;
}

bool Client::isAuthenticated() const
{
    return _authenticated;
}

void Client::appendBuffer(const std::string& data)
{
    _buffer += data;
}

bool Client::hasCompleteCommand() const
{
    return (_buffer.find("\n") != std::string::npos);
}

std::string Client::extractCommand()
{
    size_t pos = _buffer.find("\n");
    if (pos == std::string::npos)
        return "";

    std::string command = _buffer.substr(0, pos);
    _buffer.erase(0, pos + 1); // remove command + "\r\n"

    return command;
}

void Client::setNickname(const std::string& nick)
{
    _nickname = nick;
}

void Client::setUsername(const std::string& user)
{
    _username = user;
}

void Client::authenticate()
{
    _authenticated = true;
}
