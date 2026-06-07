#include "headers/server.hpp"

int main(int ac, char **av)
{
    if (ac != 3)
    {
        std::cerr << "Usage: ./ircserv <port> <password>\n";
        return 1;
    }

    int port = std::atoi(av[1]);
    std::string password = av[2];
   signal(SIGPIPE, SIG_IGN);
    try
    {
        Server server(port, password);
        server.run();
        server.stop();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}// local_1337
