#include <iostream>

#ifndef _TEST_
int __main___(int argc, char *argv[])
{
	_main__(argc, argv);
}
#endif

int _main__(int argc, char *argv[])
{
	if (argc == 1)
	{
		// default config
	}
	else if (argc == 2)
	{
		// parse
		(void)argv;
	}
	else
	{
		std::cerr << "Usage: " << argv[0] << " [path/to/file.conf]" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Hello World!" << std::endl;
	return 0;
}