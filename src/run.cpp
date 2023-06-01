#include "webserv.hpp"

static bool processRequest(Client *client, Config *config);

static std::string getServName(Config *config);

static bool isRequestComplete(Client *client);

static bool isHeaderComplete(Client *client);

static bool isChunkedBodyComplete(std::string const &body);

static void prepareErrorResponse(Client *client, Config *config, std::string const &errStr);

static void handleNewConnection(int listenSockFd, t_fdSets *fdSets, Client *clientArray[], Config *config);

void handleSockError(int fd, Config *configFromFd[], fd_set *mainFdSet,
					 Client *clientArray[])
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
	return;
}

static std::string getServName(Config *config)
{
	std::stringstream ss;

	if (config->server_name.isSome())
		ss << config->server_name.unwrap();
	else
		ss << "unnamed";
	ss << " (" << config->port << ")";
	return ss.str();
}

bool readSocket(int fd, Config *configFromFd[], t_fdSets *fdSets, Client *clientArray[])
{
	if (configFromFd[fd]) // if fd corresponds to a listening socket
	{
		handleNewConnection(fd, fdSets, clientArray, configFromFd[fd]);
		return false;
	}

	Client *client = clientArray[fd];
	Config *config = configFromFd[client->getListenFd()];
	std::cout << "=== Server " << getServName(config)
			  << ": receiving data on sock #" << fd << ":" << std::endl;

	int bufSize = MAX_HEADER_SIZE + client->getMaxBodySize() + 1;
	//int		bufSize = 6145; to test if chunked recv works
	char *buf = new char[bufSize];
	memset(buf, 0, bufSize); //only necessary for buffer visualization
	ssize_t nBytesRead = recv(fd, buf, bufSize, 0);
	if (nBytesRead <= 0)
	{
		if (nBytesRead == 0) // should add more serv and client info
			std::cout << "Client on sock #" << fd << " closed the connection." << std::endl;
		else
			std::cout << "Error on sock #" << fd << ", closing connection." << std::endl;
		close(fd);
		FD_CLR(fd, &fdSets->main);
		delete client;
		clientArray[fd] = NULL;
		delete[] buf;
		return false;
	}
	client->getRequest().append(buf, nBytesRead);
	std::cout << buf << "===" << std::endl; // tmp
	delete[] buf;
	client->setNBytesRec(nBytesRead);
    bool exit = false;
	try
	{
		exit = processRequest(client, config);
	}
	catch (std::exception const &e)
	{
		std::string errStr(e.what());
		std::cout << errStr << std::endl;
        prepareErrorResponse(client, config, errStr);
	}
    return exit;
}

// to be upgraded with file/handler
static void prepareErrorResponse(Client *client, Config *config, std::string const &errStr)
{
	HttpResponse response;

	GetRequestHandler rh(config);
	response = rh.handle_error(404, errStr); // could use specific codes for oversize requests
	client->setFlagResponse(true);
	client->setResponse(response.to_string());
	client->setFlagCloseAfterWrite(true);

	std::cout << "==== Prepared response ====" << std::endl
			  << response.to_string() << "\n==================" << std::endl; //tmp for debug
}


void writeSocket(int fd, Config *configFromFd[], t_fdSets *fdSets, Client *clientArray[])
{
	Client *c = clientArray[fd];

	if (configFromFd[fd]) // This should never happen (cannot write to listening socket)
		return; // add msg in case happens?

	ssize_t nBytesSent = send(fd, (c->getResponse()).c_str(), (c->getResponse()).size(), 0);
	if ((unsigned long) nBytesSent < (c->getResponse()).size())
		std::cout << "Server " << getServName(configFromFd[c->getListenFd()])
				  << ": error while sending data to sock #" << fd << ", request ignored."
				  << std::endl; // should add client info here
	else
		std::cout << "=== Server " << getServName(configFromFd[c->getListenFd()]) << " sent response below to sock "
				  << fd
				  << std::endl << c->getResponse() << "\n==================" << std::endl; //tmp for debug

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
	return;
}


static void handleNewConnection(int listenSockFd, t_fdSets *fdSets, Client *clientArray[], Config *config)
{
	Client *tmpClient = new Client();
	int connectSockFd;

	// Will fill client addr info and create new socket for communication
	// should check what happens when overpass FD max number
	connectSockFd = accept(listenSockFd,
						   tmpClient->getAddr(), tmpClient->getAddrSize());

	if (connectSockFd == -1)
	{
		std::cout << "Server " << getServName(config)
				  << ": could not accept new connection." << std::endl;
		delete tmpClient;
	}
	else
	{
		tmpClient->setFd(connectSockFd);
		tmpClient->setListenFd(listenSockFd);
		tmpClient->setMaxBodySize(config->client_max_body_size.unwrap());
		//tmpClient->setMaxBodySize(1); // to test body size error

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
}

int pollSockets(t_fdSets *fdSets, struct timeval *timeOut)
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

static bool processRequest(Client *client, Config *config)
{
	int max_size = MAX_HEADER_SIZE + client->getMaxBodySize();

	if (client->getNBytesRec() > max_size)
		throw std::runtime_error("Error: number of bytes read over max allowed size (HEADER + BODY)");
	else if (!isRequestComplete(client))
	{
		client->setFlagResponse(false); //should already be set to false (to verify)
		std::cout << "=== Request incomplete: server keeps reading...===" << std::endl;
		return false;
	}

	// Create a new response
	HttpResponse response;
	client->setFlagResponse(true);

	HttpRequest request((client->getHeader()).append(client->getBody()));

    bool exit = (request.get_path() == "/exit");

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
		response = delete_handler.handle_request(request);
	}
	else if (request.get_method() == Http::Methods::PUT)
	{
		PutRequestHandler put_handler(config);
		response = put_handler.handle_request(request);
	}
	else
	{
		GetRequestHandler rh(config); //nb could create a specific child error class
		response = rh.handle_error(405, "Method not allowed.");
		client->setFlagCloseAfterWrite(true);
	}

	std::cout << "==== Prepared response ====" << std::endl
			  << response.to_string() << "\n==================" << std::endl; //tmp for debug
	client->setResponse(response.to_string());
	client->clearRequest();
	client->clearHeader();
	client->clearBody();
	client->setFlagHeaderComplete(false);
    return exit;
}

// quite some unnecesarry repeated operations in current design...
static bool isRequestComplete(Client *client)
{
	if (!isHeaderComplete(client))
		return (false);

	(client->getBody()).append(client->getRequest());
	client->clearRequest();

	HttpRequest request(client->getHeader());
	if (request.get_method() != Http::Methods::POST)
		return (true); //we only accept body with POST method
	//Note, if two requests are sent in same chunk, the second one would be ignored

	std::map<std::string, std::string> headerMap = request.get_headers();

	if (headerMap.count("Content-Length"))    //note: in therory should be case insensitive
	{
		unsigned int bodySize = atoi(headerMap["Content-Length"].c_str());

		if (bodySize > client->getMaxBodySize())
            // TODO https://blog.hubspot.com/website/413-request-entity-too-large error 413 !
//			throw std::runtime_error("Error: bodysize specified in header is over maximum allowed.");
		if (client->getBody().length() >
			client->getMaxBodySize()) // this would only happen if sent body is over the size specified in header
			throw std::runtime_error("Error: bodysize is over maximum allowed.");
		if (client->getBody().length() < bodySize)
			return (false);
		return (true);
	}
	else if (headerMap.count("Transfer-Encoding")
			 && headerMap["Transfer-Encoding"].compare("chunked"))
	{
		if (client->getBody().length() > client->getMaxBodySize())
			throw std::runtime_error("Error: bodysize is over maximum allowed.");
		if (isChunkedBodyComplete(client->getBody()))
			return (true);
		return (false);
	}
	// if neither content-length nor chunk are specified, does not expect body
	return (true);
}

static bool isHeaderComplete(Client *client)
{
	if (client->getFlagHeaderComplete())
		return (true);

	std::string requestStr = client->getRequest();
	std::string header;
	std::string const ending("\r\n\r\n");
	std::string::size_type pos;

	pos = requestStr.find(ending, 0);
	if (pos != std::string::npos)
	{
		header = requestStr.substr(0, pos + ending.length());
		if (header.size() > MAX_HEADER_SIZE)
			throw std::runtime_error("Error: header size over the max allowed size.");
		client->getHeader() = header;
		std::cout << "===PARSED FULL HEADER BELOW===\n" << client->getHeader()
				  << "===" << std::endl;
		client->setFlagHeaderComplete(true);
		if (requestStr.length() > header.length())
			client->getBody() = requestStr.substr(pos + ending.length());
		client->clearRequest();
		return (true);
	}
	else if (requestStr.size() > MAX_HEADER_SIZE)
		throw std::runtime_error("Error: header size over the max allowed size.");
	return (false);
}

// Note: this not a perfect way to detect ending
// there could be some cases where ending sequence exists within chunk
static bool isChunkedBodyComplete(std::string const &body)
{
	std::string const ending("0\r\n\r\n");
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
