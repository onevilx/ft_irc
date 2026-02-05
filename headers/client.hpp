#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <unistd.h>
#include <iostream>

class Client
{
    private:
        int         _fd;
        std::string _nickname;
        std::string _username;
        std::string _buffer;
        bool        _authenticated;

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
        void        authenticate();
};

#endif
