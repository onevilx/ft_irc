#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <unistd.h>
#include <string>
#include <cstring>

class Client
{
private:
    int         _fd;
    std::string _nickname;
    std::string _username;
    std::string _buffer;
    bool        _passOk;
    bool        _nickOk;
    bool        _userOk;
    bool        _authenticated;
    bool        _isop;


public:
    Client(int fd);
    ~Client();

    int         getFd() const;
    bool        isAuthenticated() const;
    void        appendBuffer(const std::string& data);
    bool        hasCompleteCommand() const;
    std::string extractCommand();

    void        setNickname(const std::string& nick);
    void        setUsername(const std::string& user);
    const std::string& getNickname() const;
    const std::string& getUsername() const;

    void        setPassOk();
    void        setNickOk();
    void        setUserOk();
    void        tryAuthenticate();

    void        set_operator(bool status);
};

#endif
