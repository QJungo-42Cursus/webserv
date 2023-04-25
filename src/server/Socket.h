#ifndef SOCKET_H
#define SOCKET_H
#include <netinet/in.h>

class Socket
{

public:
	/* Canonic form */
	// Socket(const Socket &other);
	// Socket &operator=(const Socket &other);
	~Socket();
	Socket(int protocol, int port, int opt);

	/* Methods */
	void bind();
	void listen(int max_queue_size);
	int accept();

	/* Getters */
	int fd() const;

	int setSocketName(int sub_socket) const;

	const struct sockaddr_in & getAddress();


private:
	int _fd;
	struct sockaddr_in _address;
	socklen_t _addrlen;
	const int _protocol;
	const int _port;
	const int _opt; // options
	Socket();
};

#endif // SOCKET_H