CC =			c++
RM =			rm -f
CFLAGS =		-Wall -Wextra -Werror -std=c++98

all:
	$(CC) $(CFLAGS) server.cpp -o server
	$(CC) $(CFLAGS) client.cpp -o client
	./server & ./client

clean:
	$(RM) server client

	