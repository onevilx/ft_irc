#include "../headers/client.hpp"

// -------------------- Constructor & Destructor --------------------

Client::Client(int fd)
    : _hostname("localhost"),
      _servername("server"),
      _realname(""),
      _fd(fd),
      _nickname(""),
      _username(""),
      _buffer(""),
      _passOk(false),
      _nickOk(false),
      _userOk(false),
      _authenticated(false),
      _isop(false)
{}

Client::~Client()
{
    close(_fd);
}

// -------------------- Getters --------------------

std::string Client::getHostname() const
{
    return _hostname;
}

const std::string& Client::getServername() const
{
    return _servername;
}

const std::string& Client::getRealname() const
{
    return _realname;
}

int Client::getFd() const
{
    return _fd;
}

bool Client::isAuthenticated() const
{
    return _authenticated;
}

const std::string& Client::getNickname() const
{
    return _nickname;
}

const std::string& Client::getUsername() const
{
    return _username;
}

// -------------------- Setters for USER fields --------------------

void Client::setHostname(const std::string& host)
{
    _hostname = host;
}

void Client::setServername(const std::string& server)
{
    _servername = server;
}

void Client::setRealname(const std::string& real)
{
    _realname = real;
}

// -------------------- Buffer Handling --------------------

void Client::appendBuffer(const std::string& data)
{
    _buffer += data;
}

bool Client::hasCompleteCommand() const
{
    return (_buffer.find("\r\n") != std::string::npos) ||
           (_buffer.find("\n") != std::string::npos);
}

std::string Client::extractCommand()
{
    size_t pos;
    size_t len = 0;

    if ((pos = _buffer.find("\r\n")) != std::string::npos)
        len = 2;
    else if ((pos = _buffer.find("\n")) != std::string::npos)
        len = 1;
    else
        return "";

    std::string command = _buffer.substr(0, pos);

    if (!command.empty() && command[command.size() - 1] == '\r')
        command.erase(command.size() - 1);

    _buffer.erase(0, pos + len);
    return command;
}

// -------------------- Authentication Setters --------------------

void Client::setPassOk()
{
    _passOk = true;
}

void Client::setNickOk()
{
    _nickOk = true;
}

void Client::setUserOk()
{
    _userOk = true;
}

// -------------------- Nickname & Username --------------------

void Client::setNickname(const std::string& nick)
{
    _nickname = nick;
    _nickOk = true;
}

void Client::setUsername(const std::string& user)
{
    _username = user;
    _userOk = true;
}

// -------------------- Authentication Logic --------------------

void Client::tryAuthenticate()
{
    if (_passOk && _nickOk && _userOk)
        _authenticated = true;
}

// -------------------- Operator --------------------

void Client::set_operator(bool status)
{
    _isop = status;
}

bool        Client::get_operator(){
    return this->_isop;
}