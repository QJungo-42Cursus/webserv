CC =			c++
RM =			rm -f
CFLAGS =		-Wall -Wextra -Werror -std=c++98

all:
	$(CC) $(CFLAGS) server.cpp -o server
	$(CC) $(CFLAGS) client.cpp -o client

cpp: all
	./server & ./client

python: all
	./server & python3 client.py

curl: all
	./server & bash ./client.sh

web: all
	./server

clean:
	$(RM) server client

	