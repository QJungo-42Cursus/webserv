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
#include "http/http.h"

#ifdef _TEST_
int _main(int argc, char *argv[])
#else
int main()
#endif
{
	Socket socket(0, 8080, 1);
	socket.bind();
	socket.listen(3);
	PollFdWrapper socketPollFd(socket.fd(), PollFdWrapper::ALL);

	char buf[2000];
	int fd = -1;
	while (1)
	{
		socketPollFd.poll(-1);
		socketPollFd.log();
		std::cout << "fd : " << fd << std::endl;
		fd = socket.accept();



		memset(buf, 0, 2000);
		ssize_t l = recv(fd, buf, 1999, 0);
		// if (l == -1)continue;
		buf[l] = 0;


		// RequestHeader requestHeader(buf);
		// std::cout << requestHeader.getPath() << std::endl;
		// requestHeader.logHeaders();
		std::cout << l << " - '" << buf << "'" << std::endl;
		ResponseHeader header(Http::ResponseCode::OK, "<html><body><h2>Hello World</h2></body></html>", Http::ContentType::TEXT_HTML);
		
		ResponseHeader headerF(Http::ResponseCode::NOT_FOUND, "<html><body><h1>404</h1></body></html>", Http::ContentType::TEXT_HTML);
		std::string s_header = header.toString();
		// if (l == -1)			s_header = headerF.toString();
		send(fd, s_header.c_str(), s_header.size(), 0);
		shutdown(fd, SHUT_RDWR);
		close(fd);

	}
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