#include "webserv.hpp"

int pollSockets(t_fdSets *fdSets, struct timeval *timeOut);

void handleSockError(int fd, Config *configFromFd[], fd_set *mainFdSet, Client *clientArray[]);

bool readSocket(int fd, Config *configFromFd[], t_fdSets *fdSets, Client *clientArray[]);

void writeSocket(int fd, Config *configFromFd[], t_fdSets *fdSets, Client *clientArray[]);

int main(int argc, char **argv)
{

	if (argc > 2)
	{
		std::cerr << "Usage: " << argv[0] << " [path/to/file.conf]" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::vector<Config> configs;
	try
	{
		if (argc == 1)
			Config::parse_servers(DEFAULT_CONFIG_FILE_PATH, configs);
		else
			Config::parse_servers(argv[1], configs);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
    exit(EXIT_SUCCESS);

	std::cout << "Starting webserv with " << configs.size() << " server(s) : " << std::endl;
	for (std::vector<Config>::iterator it = configs.begin(); it != configs.end(); ++it)
	{
		std::cout << "====================" << std::endl;
		(*it).log();
		std::cout << std::endl;
	}

	if (configs.empty()) // to remove it cannot happen
		exit(EXIT_FAILURE);

	size_t nbServ = configs.size();
	int listenSockFds[nbServ];
	t_fdSets fdSets; // Struct with the sets of FDs to be monit. with select
	Config *configFromFd[FD_SETSIZE] = {}; // only valid for listenFds (1 port = 1 fixed listenFd)
	Client *clientArray[FD_SETSIZE] = {};

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
		std::string serverName = configs[iServ]->server_name.isSome() ? configs[iServ]->server_name.unwrap()
																	  : "not named";
		std::cout << "Server " << serverName << ": ready, listening on port "
				  << (configs[iServ]->port) << std::endl;
	}

    bool exit = false;

	// Main loop
	while (true)
	{
		// Poll sockets with select
		if (pollSockets(&fdSets, NULL) == 0)
		{
			std::cout << "Timeout while polling sockets with select..." << std::endl;
			continue;
		}

		// Check sockets for errors or read/write readyness
		for (int fd = 0; fd <= fdSets.fdMax; fd++)
		{
			if (FD_ISSET(fd, &fdSets.error))
				handleSockError(fd, configFromFd, &fdSets.main, clientArray);
			else if (FD_ISSET(fd, &fdSets.read) && (configFromFd[fd] || !clientArray[fd]->getFlagResponse()))
				exit = readSocket(fd, configFromFd, &fdSets, clientArray);
			else if (FD_ISSET(fd, &fdSets.write))
			{
				if (clientArray[fd] && !clientArray[fd]->getFlagResponse())
					continue;
				writeSocket(fd, configFromFd, &fdSets, clientArray);
			}
            if (exit)
                break;
		}
        if (exit)
            break;
	}

    /// Free and exit
    std::cout << "Exiting webserv..." << std::endl;
    for (std::vector<Config *>::iterator it = configs.begin(); it != configs.end(); ++it)
        delete *it;
    for (int fd = 0; fd <= fdSets.fdMax; fd++)
        if (clientArray[fd])
            delete clientArray[fd];
	return (0);
}
