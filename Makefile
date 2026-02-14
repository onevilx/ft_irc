NAME = ircserv
CC = c++
RM = rm -f
CPPFLAGS = 
SOURCES = mandatory/client.cpp mandatory/server.cpp mandatory/commands.cpp main.cpp  mandatory/mode_command.cpp  \
		mandatory/join_command.cpp mandatory/channel.cpp
INCLUDES = headers/client.hpp headers/server.hpp headers/commands.hpp headers/channel.hpp 

OBJ = $(SOURCES:.cpp=.opp)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CPPFLAGS) $(OBJ) -o $@

%.opp: %.cpp $(INCLUDES)
	$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)	

fclean: clean
	$(RM) $(NAME)

re: fclean all