NAME =     		webserv
CC =			c++
RM =			rm -f
CFLAGS =		-Wall -Wextra -Werror -std=c++98
OBJS =			$(SRCS:.cpp=.o)
SRCS =			src/main.cpp \

%.o: %.cpp
	$(CC) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
	@$(RM) $(OBJS)

fclean: clean
	@$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re

################## TESTS ##################

t: all
	./$(NAME)

T_NAME =	webserv_tests
T_CFLAGS =	-Wall -Wextra -Werror -std=c++14
T_OBJS =		$(T_SRCS:.cpp=.o)
SAN =	-fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all \
		-fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow \
		-fno-sanitize=null -fno-sanitize=alignment \
		-g

T_SRCS =		tests/main.cpp \

ifeq ($(shell uname), Linux)
GTEST =		-lgtest
LEAKS =		valgrind -q --leak-check=full --track-origins=yes
else
GTEST =		./tests/gtest/libgtest.a
LEAKS =		leaks --atExit --
endif

# san:
# $(CC) $(T_CFLAGS) $(SAN) $(SRCS) $(T_SRCS) $(GTEST) -o $(NAME)

$(T_NAME): $(T_OBJS)
	$(CC) $(T_CFLAGS) $(T_OBJS) -D _TEST_  $(SRCS) $(GTEST) -o $(T_NAME)

t_all: $(T_NAME)

t_clean:
	$(RM) $(T_OBJS)

t_fclean: t_clean
	$(RM) $(T_NAME)

t_re: t_fclean t_all

t_run: t_all
	@echo "Running tests"
	./$(T_NAME)
