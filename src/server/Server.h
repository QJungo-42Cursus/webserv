#ifndef SERVER_H
#define SERVER_H
#include "../config/Config.h"

class Server
{
public:
	/* Canonic form */
	Server();
	Server(const Server &other);
	Server &operator=(const Server &other);
	~Server();

private:
	Config _config;
	/* data */
};

#endif // SERVER_H