#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib> // atoi
#include <cstring>
#include <cctype>  // tolower

// Helper: Convert string to lowercase for case-insensitive checks
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

    // Forbidden words list
    std::string forbidden[] = {"hmar", "kelb", "stupid", "idiot", "khayb bok"};
    int forbidden_count = 5;

    // 1. Create Socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Error: Socket creation failed" << std::endl;
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    // Convert "127.0.0.1" to binary. Change if server is not local.
    if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) <= 0) {
        std::cerr << "Error: Invalid address/ Address not supported" << std::endl;
        return 1;
    }

    // 2. Connect
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Error: Connection Failed" << std::endl;
        return 1;
    }

    // 3. Authenticate
    std::string auth = "PASS " + pass + "\r\n";
    auth += "NICK " + bot_nick + "\r\n";
    auth += "USER bot 0 * :Guardian Bot\r\n";
    send(sock, auth.c_str(), auth.length(), 0);

    // 4. Main Loop with Buffer Handling
    std::string buffer = "";
    char raw_buf[4096];

    while (true) {
        int n = recv(sock, raw_buf, sizeof(raw_buf) - 1, 0);
        if (n <= 0) break; 
        raw_buf[n] = 0;
        buffer += raw_buf; 

        // Process line by line (\r\n is the delimiter)
        size_t pos;
        while ((pos = buffer.find("\r\n")) != std::string::npos) {
            std::string line = buffer.substr(0, pos);
            buffer.erase(0, pos + 2); // Remove processed line

            // std::cout << "<< " << line << std::endl; // Uncomment to debug

            // A. Handle PING (Critical for staying connected)
            if (line.find("PING") == 0) {
                std::string pong = "PONG" + line.substr(4) + "\r\n";
                send(sock, pong.c_str(), pong.length(), 0);
            }

            // B. Wait for Welcome (001) then Join
            if (!is_registered && line.find(" 001 ") != std::string::npos) {
                is_registered = true;
                std::string join = "JOIN " + chan + "\r\n";
                send(sock, join.c_str(), join.length(), 0);
                std::cout << ">> Joined " << chan << std::endl;
            }

            // C. Message Logic
            if (line.find("PRIVMSG") != std::string::npos) {
                // Parse the message content
                size_t content_pos = line.find(" :");
                if (content_pos != std::string::npos) {
                    std::string content = line.substr(content_pos + 2);
                    std::string lower_content = toLower(content);

                    // 1. Check for Bad Words
                    bool kicked = false;
                    for (int i = 0; i < forbidden_count; ++i) {
                        if (lower_content.find(forbidden[i]) != std::string::npos) {
                            
                            // Extract offender's nickname
                            size_t bang = line.find('!');
                            if (bang != std::string::npos) {
                                std::string offender = line.substr(1, bang - 1);
                                
                                // Don't kick yourself
                                if (offender != bot_nick) {
                                    std::string kick = "KICK " + chan + " " + offender + " :No bad words!\r\n";
                                    send(sock, kick.c_str(), kick.length(), 0);
                                    kicked = true;
                                }
                            }
                            break; 
                        }
                    }

                    // 2. Simple Command: !ping (Only if not kicked)
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