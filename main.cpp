#include "headers/server.hpp"


void f(){
    system("leaks ircserv");
}
int main(int ac, char **av)
{
    atexit(f);
    if (ac != 3)
    {
        std::cerr << "Usage: ./ircserv <port> <password>\n";
        return 1;
    }

    int port = std::atoi(av[1]);
    std::string password = av[2];
    try
    {
        Server server(port, password);
        server.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}