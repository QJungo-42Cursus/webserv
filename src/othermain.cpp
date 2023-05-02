#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include "server/PollFdWrapper.h"
#include "server/ResponseHeader.h"
#include "server/RequestHeader.h"
#include "server/HttpResponse.h"
#include "server/HttpRequest.h"
#include "server/RequestHandler.h"
#include "server/Socket.h"

#define PORT 8080
int main(void)
{
    int server_fd, new_socket; long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    const char *hello;
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        
        char buffer[30000] = {0};
        valread = read( new_socket , buffer, 30000);
        printf("Buffer: %s\n",buffer );
        std::string raw_request(buffer);
        HttpRequest request(raw_request);

		 RequestHandler* handler = NULL;
    switch (request.get_method()) {
        case Http::Methods::GET:
            handler = new GetRequestHandler();
            break;
        case Http::Methods::POST:
            handler = new PostRequestHandler();
            break;
        case Http::Methods::DELETE:
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
    hello = response_str.c_str();
    write(new_socket , hello , strlen(hello));
    close(new_socket);
    }
    return 0;
}