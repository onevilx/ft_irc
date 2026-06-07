NAME = ircserv
BOT_NAME = bot

CC = c++
RM = rm -f
CPPFLAGS =  -Wall -Wextra -Werror -std=c++98
SOURCES = mandatory/client.cpp mandatory/server.cpp mandatory/commands.cpp main.cpp mandatory/join_command.cpp mandatory/channel.cpp mandatory/invite_command.cpp mandatory/handleauth.cpp \
 			mandatory/mode_command.cpp  mandatory/privmsg_command.cpp mandatory/topic_command.cpp mandatory/kick_command.cpp mandatory/part_command.cpp 

BOT_SRC = bonus/bot.cpp

INCLUDES = headers/client.hpp headers/server.hpp headers/commands.hpp headers/channel.hpp 

OBJ = $(SOURCES:.cpp=.opp)
BOT_OBJ = $(BOT_SRC:.cpp=.opp)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CPPFLAGS) $(OBJ) -o $@

bonus: $(BOT_NAME)

$(BOT_NAME): $(BOT_OBJ)
	$(CC) $(CPPFLAGS) $(BOT_OBJ) -o $@

%.opp: %.cpp $(INCLUDES)
	$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ) $(BOT_OBJ)

fclean: clean
	$(RM) $(NAME) $(BOT_NAME)

re: fclean all