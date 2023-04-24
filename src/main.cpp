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
	Socket socket(0, 8080, 1);
	socket.bind();
	socket.listen();
	socket.accept();

	Header header(Header::ResponseCode::OK, "<html><body><h2>Hello World</h2></body></html>", Header::ContentType::TEXT_HTML);
	while (true)
	{
		char buf[2000];
		int l = recv(socket.fd(), buf, 1000, 0);
		buf[l] = 0;
		std::cout << buf;
		std::string s_header = header.toString();
		send(socket.fd(), s_header.c_str(), s_header.size(), 0);
	}

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