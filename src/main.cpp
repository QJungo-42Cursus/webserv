/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tplanes <tplanes@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/08 16:22:46 by tplanes           #+#    #+#             */
/*   Updated: 2023/05/09 10:47:25 by tplanes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

static int		pollSockets(t_fdSets* fdSets, struct timeval* timeOut);

static void		handleNewConnection(int listenSockFd, t_fdSets* fdSets, Client *clientArray[]);

static void		handleSockError(int fd, int listenSockFd, fd_set* mainFdSet, Client* clientArray[]);

static void		readSocket(int fd, int listenSockFd, t_fdSets* fdSets, Client* clientArray[], Config* config);

static void		writeSocket(int fd, int listenSockFd, t_fdSets* fdSets, Client* clientArray[]);

static void	processRequest(Client* client, Config *config);

static std::string	getHelloMsg(void); //tmp minimal HTTP response

static std::string	getIndexHtml(void); //tmp minimal HTTP response from index.html

static std::string	getBadRequest(void); //tmp minimal 400 Error response

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

    std::cout << "starting webserv with " << configs.size() << " server(s) : " << std::endl;
    for (std::vector<Config *>::iterator it = configs.begin(); it != configs.end(); ++it)
    {
        std::cout << "====================" << std::endl;
        (*it)->log();
        std::cout << std::endl;
    }

	int				listenSockFd; //, connectSockFd;
	t_fdSets		fdSets; // Struct with the sets of FDs to be monit. with select
	struct timeval	timeOut; // Max time for select to return if no socket has updates

	listenSockFd = getListenSock();
	FD_ZERO(&fdSets.main); // make sure it's empty
	FD_SET(listenSockFd, &fdSets.main); // adds the listening sock to the set
	fdSets.fdMax = listenSockFd;

	timeOut.tv_sec = 5;
	timeOut.tv_usec = 0;

	Client* clientArray[FD_SETSIZE] = {};

	std::cout << "Server: ready, waiting for connections..." << std::endl;
	
	// Main loop
	while (true)
	{
		// Poll sokets with select
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
				handleSockError(fd, listenSockFd, &fdSets.main, clientArray);
			else if (FD_ISSET(fd, &fdSets.read))
				readSocket(fd, listenSockFd, &fdSets, clientArray, configs[0]);
			else if (FD_ISSET(fd, &fdSets.write)) // can do read and write in same loop?
			{	
				if (clientArray[fd]->getFlagResponse() == false)
					continue ;
				writeSocket(fd, listenSockFd, &fdSets, clientArray);
			}
		}
	}
	return (0);
}

static void	handleSockError(int fd, int listenSockFd, fd_set* mainFdSet, Client* clientArray[])
{
	if (fd == listenSockFd)

	{
		std::cout << "Error on listening socket, server will reboot in 5s..."
			<< std::endl;
		// should free all clients
		exit(EXIT_FAILURE);
	}
	std::cout << "Error on socket " << fd << ": closing connection." << std::endl;
	close(fd); // needed?
	FD_CLR(fd, mainFdSet); // no need to update nfdmax?
	delete clientArray[fd];
	clientArray[fd] = NULL;
	return ;
}

static void	readSocket(int fd, int listenSockFd, t_fdSets* fdSets, Client* clientArray[], Config *config)
{

	if (fd == listenSockFd)
	{	
		handleNewConnection(listenSockFd, fdSets, clientArray);
		return ;
	}
	std::cout << "Receiving data from client on sock #" << fd << ":" << std::endl;
	int nBytesRead = recv(fd, clientArray[fd]->getRequestBuff(), BUFFSIZE, 0);
	if (nBytesRead <= 0)
	{
		if (nBytesRead == 0) 
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
	std::cout << clientArray[fd]->getRequestBuff() << std::endl; // tmp
	processRequest(clientArray[fd], config);
	return ;
}

static void	writeSocket(int fd, int listenSockFd, t_fdSets* fdSets, Client* clientArray[])
{
	Client *c = clientArray[fd];

	if (fd == listenSockFd) // This should never happen (cannot write to listening socket)
		return ;
	
	ssize_t nBytesSent = send(fd, (c->getResponse()).c_str(), (c->getResponse()).size(), 0);
	if ((unsigned long)nBytesSent < (c->getResponse()).size())
		std::cout << "Sending error: ignoring request" << std::endl;
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


static void	handleNewConnection(int listenSockFd, t_fdSets* fdSets, Client *clientArray[])
{
	Client	*tmpClient = new Client();
	int		connectSockFd;

	// Will fill client addr info and create new socket for communication
	// should check what happens when overpass FD max number
	connectSockFd = accept(listenSockFd,
		tmpClient->getAddr(), tmpClient->getAddrSize());

	if (connectSockFd == -1)
	{
		std::cout << "Could not accept new connection." << std:: endl;
		delete tmpClient;
	}
	else
	{
		tmpClient->setFd(connectSockFd);
		clientArray[connectSockFd] = tmpClient;
		// need to make sock non blocking here with fcntl?
		fcntl(connectSockFd, F_SETFL, O_NONBLOCK);
		FD_SET(connectSockFd, &fdSets->main);
		if (connectSockFd > fdSets->fdMax)
			fdSets->fdMax = connectSockFd;
		std::cout << "Server: connection from a client on socket #" << connectSockFd << std::endl;
		// build inet_ntop equiv with allowed fcts to print client info? (usless if local only)
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
		std::cout << "Error while polling with select, server will reboot in 5s..." << std::endl;
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
			response.set_version(request.get_version());
			response.set_status(405, "Method Not Allowed");
		}

	}
	std::cout << response.to_string() << std::endl; //tmp for debug
	
	//std::string tmp = response.to_string();
	//tmp.pop_back();
	//tmp.pop_back();
	//tmp.pop_back();
	//tmp = tmp + "Content-Type: text/plain; charset=utf-8\r\n\r\n";
	//std::cout << tmp << std::endl;
	//client->setResponse(tmp);
	
	//client->setResponse(getBadRequest());
	client->setResponse(response.to_string());
	client->clearRequestBuff();
	
	/*else if (strncmp(client->getRequestBuff(), "GET /index.html", 15) == 0) //to replace with proper parser
		client->setResponse(getIndexHtml());
	else if (strncmp(client->getRequestBuff(), "GET", 3) == 0) //to replace with proper parser
		client->setResponse(getHelloMsg());
	else
		client->setResponse(getBadRequest());
	*/
	return ;
}

static std::string	getHelloMsg(void) //tmp function to test sending HTTP data
{
	std::string			msg("Hello world, this is 42 WebServ!\r\n");
	std::stringstream	ss;

	ss << "HTTP/1.1 200 OK\r\n" 
		<< "Content-Length: " << msg.size() << "\r\n"
		<< "Content-Type: text/plain; charset=utf-8\r\n\r\n"
		<< msg;

	return (ss.str());
}

static std::string	getIndexHtml(void)
{
	std::stringstream	ssFile, ssOut;
	std::ifstream		fs;
	std::string			content;

	fs.open("index.html");
	ssFile << fs.rdbuf();
	content  = ssFile.str();

	ssOut << "HTTP/1.1 200 OK\r\n" 
		<< "Content-Length: " << content.size() << "\r\n"
		<< "Content-Type: text/html; charset=utf-8\r\n\r\n"
		<< content;

	return (ssOut.str());
}

static std::string	getBadRequest(void) //tmp minimal 400 Error response
{
	std::string			msg("Bad Request: either too big or non-conform to HTTP.\n");
	std::stringstream	ss;

	ss << "HTTP/1.1 400 Bad Request\r\n"
		<< "Connection: close\r\n"
		<< "Content-Length: " << msg.size() << "\r\n"
		<< "Content-Type: text/plain; charset=utf-8\r\n\r\n"
		<< msg;

	return (ss.str());
}
