#ifndef SOCKET_H
#define SOCKET_H
#include <netinet/in.h>

class Socket
{

public:
	/* Canonic form */
	Socket();
	Socket(const Socket &other);
	Socket &operator=(const Socket &other);
	~Socket();

	/* Methods */
	void create();
	void bind();
	void listen();
	void accept();
	void close();

private:
	int _server_fd;
	int _socket_fd;
	struct sockaddr_in _address;
};

#endif // SOCKET_H