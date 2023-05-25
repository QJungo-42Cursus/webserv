/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tplanes <tplanes@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/08 16:22:46 by tplanes           #+#    #+#             */
/*   Updated: 2023/05/25 13:07:32 by tplanes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "./server/request_handler/DeleteRequestHandler.h"

static int		pollSockets(t_fdSets* fdSets, struct timeval* timeOut);

static void		handleNewConnection(int listenSockFd, t_fdSets* fdSets,
	Client *clientArray[], Config* config);

static void		handleSockError(int fd, Config* configFromFd[], fd_set* mainFdSet,
	Client* clientArray[]);

static void		readSocket(int fd, Config* configFromFd[], t_fdSets* fdSets, Client* clientArray[]);

static void		writeSocket(int fd, Config* configFromFd[], t_fdSets* fdSets, Client* clientArray[]);

static void		processRequest(Client* client, Config *config);

static std::string	getServName(Config *config);

static bool	isRequestComplete(std::string const& request);

static bool	isHeaderComplete(std::string const& requestStr, std::string& header,
	std::string& body);

static bool	isChunkedBodyComplete(std::string& body);

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
		std::string serverName = configs[iServ]->server_name.isSome() ? configs[iServ]->server_name.unwrap() : "not named";
		std::cout << "Server " << serverName << ": ready, listening on port "
			<< (configs[iServ]->port) << std::endl;
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

	if (config->server_name.isSome())
		ss << config->server_name.unwrap();
	else
		ss << "unnamed";
	ss
	<< " ("
		<< config->port << ")";
	return ss.str();
}

static void	readSocket(int fd, Config* configFromFd[], t_fdSets* fdSets, Client* clientArray[])
{
	if (configFromFd[fd]) // if fd corresponds to a listening socket
	{	
		handleNewConnection(fd, fdSets, clientArray, configFromFd[fd]);
		return ;
	}
	
	Config* config = configFromFd[clientArray[fd]->getListenFd()];
	//std::cout << "=== Server " << getServName(configFromFd[clientArray[fd]->getListenFd()])
	std::cout << "=== Server " << getServName(config)
		<< ": receiving data on sock #" << fd << ":" << std::endl;
	
	//int		bufSize = MAX_HEADER_SIZE + MAX_BODY_SIZE + 1; // shld replace max body size by config value (should also protect against stack overflow?)
	
	int		maxBodySize = atoi((config->client_max_body_size.unwrap()).c_str());
	int		bufSize = MAX_HEADER_SIZE + maxBodySize + 1;
	char*	buf = new char[bufSize];

	//char	buf[MAX_HEADER_SIZE + MAX_BODY_SIZE + 1] = {}; // better to use a static one and clear it each time ?
	
	//int nBytesRead = recv(fd, clientArray[fd]->getRequestBuff(), BUFFSIZE, 0);
	int nBytesRead = recv(fd, buf, bufSize, 0);
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
		delete [] buf;
		return ;
	}
	(clientArray[fd]->getRequest()).append(buf, nBytesRead);
	clientArray[fd]->setNBytesRec(nBytesRead);
	std::cout << buf << "===" << std::endl; // tmp
	processRequest(clientArray[fd], config);
	delete [] buf;
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
	else	
		std::cout << "=== Server " << getServName(configFromFd[c->getListenFd()]) << " sent response below to sock " << fd
			<< std::endl << c-> getResponse() << "\n==================" << std::endl; //tmp for debug

	c->setFlagResponse(false);
	c->clearResponse(); // needed?
	if (c->getFlagCloseAfterWrite())
	{	
		std::cout << " === Server " << getServName(configFromFd[c->getListenFd()]) 
			<< " closing connection with sock " << fd << "===" << std::endl;
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
	int	maxBodySize = atoi((config->client_max_body_size.unwrap()).c_str());
	int	max_size = MAX_HEADER_SIZE + maxBodySize; 
	
	// Create a new response
	HttpResponse response;
	client->setFlagResponse(true); //set to true by default and switch off if necessary
	
	if (client->getNBytesRec() >= max_size || (client->getRequest()).size() >= max_size) // only last check sufficient?
	{
		response.set_version("HTTP/1.1"); // should send error page instead?
		response.set_status(400, "Bad Request: too big.");
		client->setFlagCloseAfterWrite(true); // flag to close connection after writing response
	}
	else if (!isRequestComplete(client->getRequest()))
	{
		client->setFlagResponse(false);
		std::cout << "=== Request incomplete: server keeps reading...===" << std::endl;
		return ;
	}
	else
	{	
		HttpRequest request(client->getRequest());
		
		if (request.get_method() == Http::Methods::GET) 
		{
			GetRequestHandler get_handler(config);
			response = get_handler.handle_request(request);
		}
		else if (request.get_method() == Http::Methods::POST)
		{
			PostRequestHandler post_handler(config);
			response = post_handler.handle_request(request);
		}
		else if (request.get_method() == Http::Methods::DELETE)
		{
			DeleteRequestHandler delete_handler(config);
			std::string response_str = delete_handler.handle_request_str(request);
		} 
		else
		{
			response.set_version("HTTP/1.1");
			response.set_status(405, "Method Not Allowed");
			client->setFlagCloseAfterWrite(true);
		}

	}
	std::cout << "==== Prepared response ====" << std::endl
		<< response.to_string() << "\n==================" << std::endl; //tmp for debug
	client->setResponse(response.to_string());
	client->clearRequest();
	return ;
}

// quite some unnecesarry repeated operations in current design...
// could used flags to avoid
static bool	isRequestComplete(std::string const& requestStr)
{
	std::string header, body;

	if (!isHeaderComplete(requestStr, header, body))
		return (false);

	HttpRequest	request(header);
	if (request.get_method() != Http::Methods::POST) 
		return (true); //we only accept body with POST method
	//Note, if two requests are sent in same chunk, the second one would be ignored 
	
	std::map<std::string, std::string> headerMap = request.get_headers();
	
	if (headerMap.count("Content-Length"))	//note: in therory should be case insensitive 
	{
		unsigned int bodySize = atoi(headerMap["Content-Length"].c_str()); // return err here 
																	     // if >maxbody size?
		// leaves error treatment to Quentin's parser atm
		if (bodySize > MAX_BODY_SIZE)
			bodySize = MAX_BODY_SIZE;
		if (body.length() < bodySize)
			return (false);
		return (true);
	}	
	else if (headerMap.count("Transfer-Encoding")
		&& headerMap["Transfer-Encoding"].compare("chunked"))	
	{
		if (isChunkedBodyComplete(body))
			return (true);
		return (false);
	}
	// if neither content-length nor chunk are specified, does not expect body 
	return (true);	
}

static bool	isHeaderComplete(std::string const& requestStr, std::string& header,
	std::string& body)
{
	std::string const	ending("\r\n\r\n");
	std::string::size_type pos;
	
	//if (requestStr.length() < ending.length())
	//	return (false);
	//if (request.compare(request.length() - ending.length(), ending.length(), ending) == 0)
	
	pos = requestStr.find(ending, 0);
	if (pos != std::string::npos)
	{
		header = requestStr.substr(0, pos + ending.length());
		std::cout << "===PARSED FULL HEADER BELOW===\n" << header << "===" << std::endl;
		if (requestStr.length() > header.length())
			body = requestStr.substr(pos + ending.length());
		return (true);
	}
	return (false);
}

// Note: this not a perfect way to detect ending
// there could be some cases where ending sequence exists within chunk 
static bool	isChunkedBodyComplete(std::string& body)
{
	std::string const	ending("0\r\n\r\n");
	std::string::size_type pos;
	
	pos = body.find(ending, 0);
	if (pos != std::string::npos)
	{
		std::cout << "===END OF CHUNKED BODY (BELOW) DETECTED===\n"
			<< body << "===" << std::endl;
		return (true);
	}
	return (false);
}
