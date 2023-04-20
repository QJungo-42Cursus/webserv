#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include "server/Header.h"
#include "server/Socket.h"

#ifdef _TEST_
int _main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
	Header header(Header::OK, "<html><body><h1>Hello World</h1></body></html>", Header::TEXT_HTML);
	// std::cout << header.getHeader() << std::endl;

	Socket socket(0, 8080, 1);
	socket.bind();
	socket.listen();
	socket.accept();

	char buf[2000];
	int l = recv(socket.getSocketFd(), buf, 1000, 0);
	buf[l] = 0;
	std::cout << buf;
	send(socket.getSocketFd(), header.getHeader().c_str(), header.getHeader().size(), 0);

	socket.close();

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