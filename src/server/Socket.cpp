#include "Socket.h"
#include <sys/socket.h>
#include <exception>
#include <unistd.h>
#include <iostream>

Socket::~Socket()
{
	// TODO check s'il est closed ?
}

Socket::Socket(int protocol, int port, int opt) : _protocol(protocol), _port(port), _opt(opt) {
	/// Creating socket file descriptor
	_server_fd = socket(AF_INET, SOCK_STREAM, _protocol);
	if (_server_fd == -1)
	{
		std::cerr << "caca" << std::endl;
		throw std::exception();
	}

	/// Forcefully attaching socket to the port 8080
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &_opt, sizeof(_opt)))
	{
		throw std::exception();
	}
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(_port);
}

void Socket::bind()
{
	int res = ::bind(_server_fd, (struct sockaddr *)&_address,
					 sizeof(_address));
	if (res == -1)
	{
		throw std::exception();
	}
}

void Socket::listen()
{
	int max_queue_size = 3; // TODO
	int res = ::listen(_server_fd, max_queue_size);
	if (res == -1)
	{
		throw std::exception();
	}
}

void Socket::accept()
{
	int addrlen = sizeof(_address);
	_socket_fd = ::accept(_server_fd, (struct sockaddr *)&_address,
						  (socklen_t *)&addrlen);
	if (_socket_fd == -1)
	{
		throw std::exception();
	}
}

void Socket::close()
{
	// TODO juste le mettre dans le destructeur
	::close(_socket_fd);
	shutdown(_server_fd, SHUT_RDWR);
}

int Socket::fd() const 
{
	return _socket_fd;
}