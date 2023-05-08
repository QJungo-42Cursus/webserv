NAME = webserv

SRCS := webserv.cpp listenerSocket.cpp Client.cpp 

OBJS := $(SRCS:%.cpp=%.o)

CC := c++

CPPFLAGS := -Wall -Wextra -Werror -std=c++98

RM := rm -f

%.o : %.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

#############################################

$(NAME) : $(OBJS)
	$(CC) $(CPPFLAGS) $(OBJS) -o $@

all : $(NAME)

clean :
	$(RM) $(OBJS)

fclean : clean
	$(RM) $(NAME)

re : fclean all

.PHONY : all clean fclean re
