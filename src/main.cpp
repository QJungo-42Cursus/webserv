#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include "config/Config.h"

#define DEFAULT_CONFIG_FILE_PATH "./config/default.yaml"


#ifdef _TEST_
int _main(int argc, char *argv[])
#else

int main(int argc, char *argv[])
#endif
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

	// ...
	// program logic
	// ...

	/// free configs
	for (std::vector<Config *>::iterator it = configs.begin(); it != configs.end(); ++it)
		delete *it;
	return 0;
}
