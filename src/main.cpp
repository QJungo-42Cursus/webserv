#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include "server/PollFdWrapper.h"
#include "server/ResponseHeader.h"
#include "server/RequestHeader.h"
#include "server/HttpResponse.h"
#include "server/HttpRequest.h"
#include "server/RequestHandler.h"
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

		const char* raw_request_char = "GET /another.html HTTP/1.1\r\nHost: localhost\r\n\r\n";
		std::string raw_request(raw_request_char);
		std::cout << raw_request << std::endl;
		HttpRequest request(raw_request);

		 RequestHandler* handler = NULL;
    switch (request.get_method()) {
        case GET:
            handler = new GetRequestHandler();
            break;
        case POST:
            handler = new PostRequestHandler();
            break;
        case DELETE:
            handler = new DeleteRequestHandler();
            break;
        default:
            std::cout << "Unsupported HTTP method" << std::endl;
			break;
    }

    // Handle the request and obtain the HttpResponse
    HttpResponse response = handler->handle_request(request);

    // Delete the handler instance
    delete handler;

    // Convert the HttpResponse back to a string
    std::string response_str = response.to_string();
	//std::cout << "Reponse: " << std::endl;
	//std::cout << response_str << std::endl;
	send(fd, response_str.c_str(), response_str.size(), 0);
	shutdown(fd, SHUT_RDWR);
	close(fd);
	}
	return 0;
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