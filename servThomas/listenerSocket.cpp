/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   listenerSocket.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tplanes <tplanes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 15:32:59 by tplanes           #+#    #+#             */
/*   Updated: 2023/05/04 16:37:50 by tplanes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

static void		fillServInfo(struct addrinfo** serverInfo);

static int		bindSocket(struct addrinfo* serverInfo);

int	getListenSock(void)
{
	int					listenSockFd;
	struct addrinfo*	serverInfo; // linked list of server info (to be filled by getaddrinfo())

	fillServInfo(&serverInfo);
	listenSockFd = bindSocket(serverInfo);
	freeaddrinfo(serverInfo); 
	if (listenSockFd == -1)
	{
		std::cout << "Error: could not bind listening socket" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (listen(listenSockFd, BACKLOG) == -1)
	{
		std::cout << "Error: could not put socket in listen mode" << std::endl;
		exit(EXIT_FAILURE);
	}
	// need to make sock non blocking here with fcntl?
	if(fcntl(listenSockFd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cout << "Error: could not put listening socket in non-blocking  mode" << std::endl;
		exit(EXIT_FAILURE);
	}
	return (listenSockFd);
}

static void	fillServInfo(struct addrinfo** serverInfo)
{
	struct addrinfo		hints; // Connection type(s) and IP type(s) we allow
	
	memset(&hints, 0, sizeof(hints)); // init to empty
	hints.ai_family = AF_UNSPEC;     // either IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // only TCP stream sockets (no UDP allowed)
	hints.ai_flags = AI_PASSIVE; // flag to create a listening socket

	// first param NULL for local host
	if (int retVal = getaddrinfo(NULL, PORT, &hints, serverInfo) != 0) 
	{	
		std::cout << "Error using getaddrinfo: " << gai_strerror(retVal) << std::endl;
		exit(EXIT_FAILURE); //free and exit
	}
	return ;
}

// Loop through all returned addrinfo structs and try to init and bind the listening socket
static int	bindSocket(struct addrinfo* serverInfo)
{
	int	listenSockFd;
	int yes = 1;

	struct addrinfo *infoPtr = NULL;
	for (infoPtr = serverInfo; infoPtr != NULL; infoPtr = infoPtr->ai_next)
	{
		listenSockFd = socket(infoPtr->ai_family, infoPtr->ai_socktype, infoPtr->ai_protocol);
		if (listenSockFd == -1)
		{
			std::cout << "Warning: could not initiate listening socket on specified "
				"parameter set, trying with next set..." << std::endl;
			continue ;
		}
		// to avoid waiting for a previous connection to clear before reusing address
		if (setsockopt(listenSockFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) 
		{
			std::cout << "Warning: could not allow address reuse for listening socket"
				"trying next one." << std::endl;
			close(listenSockFd);
			continue ;
		} 
		if (bind(listenSockFd, infoPtr->ai_addr, infoPtr->ai_addrlen) == -1)
		{
			std::cout << "Warning: could not bind listening socket, "
				"trying next one" << std::endl;
			close(listenSockFd);
			continue ;
		}
		else
			break ;
	}
	if (infoPtr == NULL)
		return (-1);
	return (listenSockFd);
}
