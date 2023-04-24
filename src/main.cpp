#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include "server/PollFdWrapper.h"
#include "server/ResponseHeader.h"
#include "server/RequestHeader.h"

#include "server/Socket.h"
#include <poll.h>
#include <errno.h>

#ifdef _TEST_
int _main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
	Socket socket(0, 8080, 1);
	socket.bind();
	socket.listen();
	socket.accept(); // blocking until a client connects
	PollFdWrapper n(socket.fd(), POLLIN);
	ResponseHeader header(ResponseHeader::ResponseCode::OK, "<html><body><h2>Hello World</h2></body></html>", ResponseHeader::ContentType::TEXT_HTML);
	while (true)
	{
		try
		{
			n.poll(1000);
		}
		catch (std::exception &e)
		{
			break;
		}
		if (!n.isIn())
		{
			std::cout << "not isIN" << std::endl;
			continue;
		}

		char buf[2000];
		int l = recv(socket.fd(), buf, 1000, 0);
		buf[l] = 0;
		if (l == 0)
		{
			std::cout << "client disconnected" << std::endl;
			break;
		}
		RequestHeader repHeader = RequestHeader(std::string(buf));
		if (repHeader._path == "/exit")
			break;
		std::cout << buf;
		std::string s_header = header.toString();
		send(socket.fd(), s_header.c_str(), s_header.size(), 0);
		// socket.listen();
		// socket.accept(); // blocking until a client connects
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