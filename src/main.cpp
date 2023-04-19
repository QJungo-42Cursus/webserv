#include <iostream>
#include <cstdlib>
#include "server/Header.h"

#ifdef _TEST_
int _main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
	Header header(Header::OK, "<html><body><h1>Hello World</h1></body></html>", Header::TEXT_HTML);
	std::cout << header.getHeader() << std::endl;
	return 0;

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
	return 0;
}