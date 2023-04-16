NAME =     		webserv
CC =			c++
RM =			rm -f
SRCS =			main.cpp
CFLAGS =		-Wall -Wextra -Werror -std=c++98
OBJS =			$(SRCS:.cpp=.o)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@


all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
	@$(RM) $(OBJS)

fclean: clean
	@$(RM) $(NAME)

re: fclean all

################## TESTS ##################

t: all
	./$(NAME)

.PHONY: all clean fclean re
