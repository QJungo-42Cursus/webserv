/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tplanes <tplanes@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/08 16:22:46 by tplanes           #+#    #+#             */
/*   Updated: 2023/05/10 12:03:12 by tplanes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

static int		pollSockets(t_fdSets* fdSets, struct timeval* timeOut);

static void		handleNewConnection(int listenSockFd, t_fdSets* fdSets, Client *clientArray[], Config* config);

static void		handleSockError(int fd, Config* configFromFd[], fd_set* mainFdSet, Client* clientArray[]);

static void		readSocket(int fd, Config* configFromFd[], t_fdSets* fdSets, Client* clientArray[]);

static void		writeSocket(int fd, Config* configFromFd[], t_fdSets* fdSets, Client* clientArray[]);

static void		processRequest(Client* client, Config *config);

static std::string	getServName(Config *config);

int main(int argc, char **argv)
{

    if (argc > 2)
    {
        std::cerr << "Usage: " << argv[0] << " [path/to/file.conf]" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::vector<Config *> configs;
    try
    {
        configs = argc == 1 ? Config::parse_servers(DEFAULT_CONFIG_FILE_PATH) : Config::parse_servers(argv[1]);
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Starting webserv with " << configs.size() << " server(s) : " << std::endl;
    for (std::vector<Config *>::iterator it = configs.begin(); it != configs.end(); ++it)
    {
        std::cout << "====================" << std::endl;
        (*it)->log();
        std::cout << std::endl;
    }

	if (configs.size() == 0) // to remove if cannot happen
		exit(EXIT_FAILURE);

	int				nbServ = configs.size();	
	int				listenSockFds[nbServ];
	t_fdSets		fdSets; // Struct with the sets of FDs to be monit. with select
	struct timeval	timeOut; // Max time for select to return if no socket has updates
	Config*			configFromFd[FD_SETSIZE] = {}; // only valid for listenFds (1 port = 1 fixed listenFd)
	Client*			clientArray[FD_SETSIZE] = {};

	timeOut.tv_sec = 5;
	timeOut.tv_usec = 0;
	
	FD_ZERO(&fdSets.main); // make sure it's empty
	fdSets.fdMax = 0;

	// Get one listening socket per "server"
	for (int iServ = 0; iServ < nbServ; iServ++)
	{
		listenSockFds[iServ] = getListenSock(configs[iServ]);
		configFromFd[listenSockFds[iServ]] = configs[iServ];
		FD_SET(listenSockFds[iServ], &fdSets.main); // adds the listening sock to the set
		if (listenSockFds[iServ] > fdSets.fdMax)
			fdSets.fdMax = listenSockFds[iServ];
		std::cout << "Server " << (configs[iServ]->server_name).unwrap() << ": ready, listening on port "
			<< (configs[iServ]->port).unwrap() << std::endl;
	}

	// Main loop
	while (true)
	{
		// Poll sockets with select
		// if (pollSockets(&fdSets, &timeOut) == 0)
		(void)timeOut;
		if (pollSockets(&fdSets, NULL) == 0)
		{
			std::cout << "Timeout while polling sockets with select..." << std::endl;
			continue ;
		}
		
		// Check sockets for errors or read/write readyness
		for (int fd = 0; fd <= fdSets.fdMax; fd++)
 		{
			if (FD_ISSET(fd, &fdSets.error))
				handleSockError(fd, configFromFd, &fdSets.main, clientArray);
			else if (FD_ISSET(fd, &fdSets.read))
				readSocket(fd, configFromFd, &fdSets, clientArray);
			else if (FD_ISSET(fd, &fdSets.write)) // can do read and write in same loop?
			{	
				if (clientArray[fd] && clientArray[fd]->getFlagResponse() == false)
					continue ;
				writeSocket(fd, configFromFd, &fdSets, clientArray);
			}
		}
	}
	return (0);
}

static void	handleSockError(int fd, Config* configFromFd[], fd_set* mainFdSet, Client* clientArray[])
{
	if (configFromFd[fd]) // if fd is a listening socket
	{
		std::cout << "Server " << getServName(configFromFd[fd]) 
			<< ": error on listening socket, will shutdown in 5s..."
			<< std::endl;
		// should free all clients ?
		sleep(5);
		exit(EXIT_FAILURE); // make it reboot instead?
	}
	std::cout << "Server " << getServName(configFromFd[clientArray[fd]->getListenFd()]) 
		<< ": error on socket " << fd << ": closing connection." << std::endl;
	close(fd); // needed?
	FD_CLR(fd, mainFdSet); // no need to update nfdmax?
	delete clientArray[fd];
	clientArray[fd] = NULL;
	return ;
}

static std::string	getServName(Config* config)
{
	std::stringstream	ss;

	ss << config->server_name.unwrap() << " (" 
		<< config->port.unwrap() << ")";
	return ss.str();
}

static void	readSocket(int fd, Config* configFromFd[], t_fdSets* fdSets, Client* clientArray[])
{
	if (configFromFd[fd]) // if fd corresponds to a listening socket
	{	
		handleNewConnection(fd, fdSets, clientArray, configFromFd[fd]);
		return ;
	}
	std::cout << "=== Server " << getServName(configFromFd[clientArray[fd]->getListenFd()])
		<< ": receiving data on sock #" << fd << ":" << std::endl;
	
	int nBytesRead = recv(fd, clientArray[fd]->getRequestBuff(), BUFFSIZE, 0);
	if (nBytesRead <= 0)
	{
		if (nBytesRead == 0) // should add more serv and client info
			std::cout << "Client on sock #" << fd << " closed the connection." << std::endl;
		else
			std::cout << "Error on sock #" << fd << ", closing connection." << std::endl;
		close(fd);
		FD_CLR(fd, &fdSets->main);
		delete clientArray[fd];
		clientArray[fd] = NULL;
		return ;
	}
	clientArray[fd]->setNBytesRequest(nBytesRead);
	std::cout << clientArray[fd]->getRequestBuff() << "===" << std::endl; // tmp
	processRequest(clientArray[fd], configFromFd[clientArray[fd]->getListenFd()]);
	return ;
}

static void	writeSocket(int fd, Config* configFromFd[], t_fdSets* fdSets, Client* clientArray[])
{
	Client *c = clientArray[fd];

	if (configFromFd[fd]) // This should never happen (cannot write to listening socket)
		return ; // add msg in case happens?
	
	ssize_t nBytesSent = send(fd, (c->getResponse()).c_str(), (c->getResponse()).size(), 0);
	if ((unsigned long)nBytesSent < (c->getResponse()).size())
		std::cout << "Server " << getServName(configFromFd[c->getListenFd()]) 
			<< ": error while sending data to sock #" << fd << ", request ignored." << std::endl; // should add client info here
	c->setFlagResponse(false);
	c->clearResponse(); // needed?
	if (c->getFlagCloseAfterWrite())
	{	
		close(fd);
		FD_CLR(fd, &fdSets->main);
		delete c;
		c = NULL;
	}
	return ;
}


static void	handleNewConnection(int listenSockFd, t_fdSets* fdSets, Client *clientArray[], Config *config)
{
	Client	*tmpClient = new Client();
	int		connectSockFd;

	// Will fill client addr info and create new socket for communication
	// should check what happens when overpass FD max number
	connectSockFd = accept(listenSockFd,
		tmpClient->getAddr(), tmpClient->getAddrSize());

	if (connectSockFd == -1)
	{
		std::cout << "Server " << getServName(config) 
			<< ": could not accept new connection." << std:: endl;
		delete tmpClient;
	}
	else
	{
		tmpClient->setFd(connectSockFd);
		tmpClient->setListenFd(listenSockFd);
		clientArray[connectSockFd] = tmpClient;
		// need to make sock non blocking here with fcntl?
		fcntl(connectSockFd, F_SETFL, O_NONBLOCK);
		FD_SET(connectSockFd, &fdSets->main);
		if (connectSockFd > fdSets->fdMax)
			fdSets->fdMax = connectSockFd;
		std::cout << "Server " << getServName(config) 
			<< ": connection from a client on socket #" << connectSockFd << std::endl;
		// build inet_ntop equiv with allowed fcts to print client info? (useless if local only)
	}
	return ;
}

static int	pollSockets(t_fdSets* fdSets, struct timeval* timeOut)
{
	// Should check that this does a proper deep copy
	fdSets->read = fdSets->write = fdSets->error = fdSets->main;

	// Checks which socks are ready for read/write or error
	int selectRetVal = select(fdSets->fdMax + 1, &fdSets->read, &fdSets->write,
		&fdSets->error, timeOut);
	if (selectRetVal == -1)
	{
		std::cout << "Error while polling with select, server will shutdown in 5s..." << std::endl;
		sleep(5);
		exit(EXIT_FAILURE); // (should free Clients first) should implement main proc to reboot...
	}
	return (selectRetVal);
}

static void	processRequest(Client* client, Config *config)
{
	client->setFlagResponse(true);
	//client->setFlagCloseAfterWrite(true); // flag to close connection after writing response
	
	// Create a new response
	HttpResponse response;
	
	if (client->getNBytesRequest() >= BUFFSIZE)
		response.set_status(400, "Bad Request: too big.");
	else 
	{
		std::string rawRequest(client->getRequestBuff());
		HttpRequest request(rawRequest);

		if (request.get_method() == Http::Methods::GET) 
		{
			GetRequestHandler get_handler(config);
			response = get_handler.handle_request(request); //error or bad request not showing up on browser, need more info or content !=0
		}
		else if (request.get_method() == Http::Methods::POST)
		{
			PostRequestHandler post_handler(config);
			response = post_handler.handle_request(request);
		}
		else if (request.get_method() == Http::Methods::DELETE)
		{
			DeleteRequestHandler delete_handler(config);
			response = delete_handler.handle_request(request);
		} 
		else
		{
			response.set_version("HTTP/1.1");
			response.set_status(405, "Method Not Allowed");
		}

	}
	std::cout << "==== Response ====" << std::endl << response.to_string() << "\n==================" << std::endl; //tmp for debug
	client->setResponse(response.to_string());
	client->clearRequestBuff();
	return ;
}
