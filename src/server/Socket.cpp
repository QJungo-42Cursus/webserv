#include "Socket.h"
#include <sys/socket.h>
#include <exception>
#include <unistd.h>

void Socket::create()
{
	int protocol = 0; // TODO
	int port = 8080;  // TODO
	int opt = 1;	  // TODO

	/// Creating socket file descriptor
	_server_fd = socket(AF_INET, SOCK_STREAM, protocol);
	if (_server_fd == -1)
	{
		throw std::exception();
	}

	/// Forcefully attaching socket to the port 8080
	if (setsockopt(_server_fd, SOL_SOCKET,
				   SO_REUSEADDR | SO_REUSEPORT, &opt,
				   sizeof(opt)))
	{
		throw std::exception();
	}
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(port);
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
	::close(_socket_fd);
	shutdown(_server_fd, SHUT_RDWR);
}
