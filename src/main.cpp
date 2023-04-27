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
	std::vector<Config> configs;

	if (argc == 1)
		configs = Config::parse("config/default.yaml");
	else if (argc == 2)
		configs = Config::parse(argv[1]);
	else
	{
		std::cerr << "Usage: " << argv[0] << " [path/to/file.conf]" << std::endl;
		exit(EXIT_FAILURE);
	}
	return 0;
}
