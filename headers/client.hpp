#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <unistd.h>
#include <string>
#include <cstring>
#include <arpa/inet.h>     // inet_aton, htonl, INADDR_LOOPBACK
#include <netdb.h>         // gethostbyaddr, hostent
#include <netinet/in.h>    // struct in_addr
#include <sys/socket.h>    // AF_INE

class Client
{
private:
    std::string _hostname;
    std::string _servername;
    std::string _realname;

    int         _fd;
    std::string _username;
    std::string _nickname;
    std::string _buffer;
    std::string _ipaddress;

    bool        _passOk;
    bool        _nickOk;
    bool        _userOk;
    bool        _authenticated;
    bool        _isop;

public:
    Client(int fd);
    ~Client();

    int         getFd() const;

    // ---- Host / Identity ----
    std::string getHostname() const;
    const std::string& getServername() const;
    const std::string& getRealname() const;

    bool        isAuthenticated() const;
    bool        isPassOk() const;

    void        appendBuffer(const std::string& data);
    bool        hasCompleteCommand() const;
    std::string extractCommand();

    // ---- Nick / User ----
    void        setNickname(const std::string& nick);
    void        setUsername(const std::string& user);
    void        setHostname(const std::string& host);
    void        setServername(const std::string& server);
    void        setRealname(const std::string& real);

    const std::string& getNickname() const;
    const std::string& getUsername() const;

    // ---- Authentication ----
    void        setPassOk();
    void        setNickOk();
    void        setUserOk();
    void        tryAuthenticate();

    // ---- Operator ----
    void        set_operator(bool status);
    bool        get_operator();
    
    //hostname resolution
    void setIpAddress(const std::string& ip);
    std::string getIpAddress() const;
    std::string get_client_host() const;
};

#endif
