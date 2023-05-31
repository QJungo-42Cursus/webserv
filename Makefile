NAME =     		webserv
CC =			c++
RM =			rm -f
#CFLAGS =		-std=c++98 -I ./http/ -g3 -fsanitize=address
CFLAGS =		-std=c++98 -I ./http/
OBJS =			$(SRCS:.cpp=.o)
SRCS =			src/main.cpp \
				src/config/CgiConfig.cpp \
				src/config/Config.cpp \
				src/config/Route.cpp \
				src/config/yaml_helper.cpp \
				src/utils/split.cpp \
				src/utils/get_cwd.cpp \
				src/server/HttpResponse.cpp \
				src/server/HttpRequest.cpp \
				src/server/request_handler/RequestHandler.cpp \
				src/server/request_handler/DeleteRequestHandler.cpp \
				src/server/request_handler/GetRequestHandler.cpp \
				src/server/request_handler/PostRequestHandler.cpp \
				src/server/request_handler/PutRequestHandler.cpp \
				src/server/request_handler/utils.cpp \
				src/server/Client.cpp \
				src/server/listenerSocket.cpp \
				src/cgi_executor/CgiExecutor.cpp \


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

.PHONY: all clean fclean re