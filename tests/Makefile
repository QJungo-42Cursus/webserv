NAME =		webserv_tests
CC =		c++
CFLAGS =	-Wall -Wextra -Werror -std=c++14
RM =		rm -f
GTEST =		./libgtest.a
OBJS =		$(SRCS:.cpp=.o)
SAN =	-fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all \
		-fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow \
		-fno-sanitize=null -fno-sanitize=alignment \
		-g

SRCS =		main.cpp \

ifeq ($(shell uname), Linux)
LEAKS =		valgrind -q --leak-check=full --track-origins=yes
else
LEAKS =		leaks --atExit --
endif

san:
	$(CC) $(CFLAGS) $(SAN) $(SRCS) $(GTEST) $(LIBFT_A) -o $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(SAN) $(OBJS) $(GTEST) $(LIBFT_A) -o $(NAME)

all: $(NAME)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

run:
	@echo "Running tests with sanitizers"
	make san
	./$(NAME)
