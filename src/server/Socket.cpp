#include "Socket.h"
#include <sys/socket.h>
#include <exception>
#include <unistd.h>
#include <iostream>

Socket::~Socket()
{
	shutdown(_fd, SHUT_RDWR);
}

Socket::Socket(int protocol, int port, int opt) : _protocol(protocol), _port(port), _opt(opt)
{
	/// Creating socket file descriptor
	_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, _protocol);
	if (_fd == -1)
	{
		std::cerr << "caca" << std::endl;
		throw std::exception();
	}

	/// Forcefully attaching socket to the port 8080
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &_opt, sizeof(_opt)))
	{
		throw std::exception();
	}
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(_port);
}

void Socket::bind()
{
	if (::bind(_fd, (struct sockaddr *)&_address, sizeof(_address)) == -1)
	{
		throw std::exception();
	}
}

void Socket::listen(int max_queue_size)
{
	if (::listen(_fd, max_queue_size) == -1)
	{
		throw std::exception();
	}
}

int Socket::accept()
{
	socklen_t addrlen = sizeof(_address);
	int socket_fd = ::accept(_fd, (struct sockaddr *)&_address, &addrlen);
	if (socket_fd == -1)
	{
		throw std::exception();
	}
	return socket_fd;
}

int Socket::fd() const
{
	return _fd;
}