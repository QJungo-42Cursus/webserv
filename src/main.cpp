#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include "server/PollFdWrapper.h"
#include "server/ResponseHeader.h"
#include "server/RequestHeader.h"

#include "server/Socket.h"
#include <poll.h>
#include <errno.h>
#include <string.h>

#ifdef _TEST_
int _main(int argc, char *argv[])
#else
int main()
#endif
{
	Socket socket(0, 8080, 1);
	socket.bind();
	socket.listen(3);

	PollFdWrapper pollFd(socket.fd(), POLLIN);

	while (1)
	{
		pollFd.poll(-1);
		int fd = socket.accept();
		ResponseHeader header(ResponseHeader::ResponseCode::OK, "<html><body><h2>Hello World</h2></body></html>", ResponseHeader::ContentType::TEXT_HTML);

		char buf[2000];
		int l = recv(fd, buf, 1000, 0);
		buf[l] = 0;
		std::cout << buf;
		std::string s_header = header.toString();
		send(fd, s_header.c_str(), s_header.size(), 0);
		close(fd);
	}

	socket.close();
}

/*
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
*/