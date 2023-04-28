#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include "config/Config.h"

#ifdef _TEST_
int _main(int argc, char *argv[])
#else

int main(int argc, char *argv[])
#endif
{
	std::vector<Config *> configs;
	if (argc == 1)
		configs = Config::parse_servers("/home/qjungo/Cursus/webserv/config/default.yaml");
	else if (argc == 2)
		configs = Config::parse_servers(argv[1]);
	else
	{
		std::cerr << "Usage: " << argv[0] << " [path/to/file.conf]" << std::endl;
		exit(EXIT_FAILURE);
	}

	// free configs
	for (std::vector<Config *>::iterator it = configs.begin(); it != configs.end(); ++it)
		delete *it;
	return 0;
}
