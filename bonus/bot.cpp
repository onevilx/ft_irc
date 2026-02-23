#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cctype>

std::string toLower(std::string s) {
    for (size_t i = 0; i < s.length(); ++i)
        s[i] = std::tolower(s[i]);
    return s;
}

int main(int ac, char **av) {
    if (ac != 4) {
        std::cerr << "Usage: ./bot <port> <pass> <channel>" << std::endl;
        return 1;
    }

    int port = std::atoi(av[1]);
    std::string pass = av[2];
    std::string chan = av[3];
    std::string bot_nick = "kicker";
    bool is_registered = false; 

    std::string forbidden[] = {"hmar", "kelb", "stupid", "idiot", "khayb bok"};
    int forbidden_count = 5;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Error: Socket creation failed" << std::endl;
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) <= 0) {
        std::cerr << "Error: Invalid address/ Address not supported" << std::endl;
        return 1;
    }

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Error: Connection Failed" << std::endl;
        return 1;
    }

    std::string auth = "PASS " + pass + "\r\n";
    auth += "NICK " + bot_nick + "\r\n";
    auth += "USER bot 0 * :Guardian Bot\r\n";
    send(sock, auth.c_str(), auth.length(), 0);

    std::string buffer = "";
    char raw_buf[4096];

    while (true) {
        int n = recv(sock, raw_buf, sizeof(raw_buf) - 1, 0);
        if (n <= 0) break; 
        raw_buf[n] = 0;
        buffer += raw_buf; 

        size_t pos;
        while ((pos = buffer.find("\r\n")) != std::string::npos) {
            std::string line = buffer.substr(0, pos);
            buffer.erase(0, pos + 2); 
            if (line.find("PING") == 0) {
                std::string pong = "PONG" + line.substr(4) + "\r\n";
                send(sock, pong.c_str(), pong.length(), 0);
            }

            if (!is_registered && line.find(" 001 ") != std::string::npos) {
                is_registered = true;
                std::string join = "JOIN " + chan + "\r\n";
                send(sock, join.c_str(), join.length(), 0);
                std::cout << ">> Joined " << chan << std::endl;
            }

            if (line.find("PRIVMSG") != std::string::npos) {
                size_t content_pos = line.find(" :");
                if (content_pos != std::string::npos) {
                    std::string content = line.substr(content_pos + 2);
                    std::string lower_content = toLower(content);

                    bool kicked = false;
                    for (int i = 0; i < forbidden_count; ++i) {
                        if (lower_content.find(forbidden[i]) != std::string::npos) {
                            
                            size_t bang = line.find('!');
                            if (bang != std::string::npos) {
                                std::string offender = line.substr(1, bang - 1);
                                
                                if (offender != bot_nick) {
                                    std::string kick = "KICK " + chan + " " + offender + " :No bad words!\r\n";
                                    send(sock, kick.c_str(), kick.length(), 0);
                                    kicked = true;
                                }
                            }
                            break; 
                        }
                    }

                    if (!kicked && lower_content.find("!ping") == 0) {
                         std::string reply = "PRIVMSG " + chan + " :Pong! I am watching you.\r\n";
                         send(sock, reply.c_str(), reply.length(), 0);
                    }
                }
            }
        }
    }
    close(sock);
    return 0;
}