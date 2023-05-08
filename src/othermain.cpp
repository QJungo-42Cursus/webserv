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
#include "Config.h"
#define DEFAULT_CONFIG_FILE_PATH "./config/default.yaml"
#define PORT 8080

int main(void)
{
  // Load the configuration from a YAML file
    std::string config_file_path = "./config/default.yaml";
    std::vector<Config*> configs = Config::parse_servers(config_file_path);
    
    // Assume that we are using the first config in the vector
    Config* config = configs[0];
    config->log();
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
        // Create a new response
        HttpResponse response;

    if (request.get_method() == Http::Methods::GET) {
        GetRequestHandler get_handler(config);
        response = get_handler.handle_request(request);
    } else if (request.get_method() == Http::Methods::POST) {
        PostRequestHandler post_handler(config);
        response = post_handler.handle_request(request);
    } else if (request.get_method() == Http::Methods::DELETE) {
        DeleteRequestHandler delete_handler(config);
        response = delete_handler.handle_request(request);
    } else {
        response.set_version(request.get_version());
        response.set_status(405, "Method Not Allowed");
    }

    // Convert the HttpResponse back to a string
    std::string response_str = response.to_string();
    std::cout << "Response:" << std::endl;
    std::cout << response_str << std::endl;
    hello = response_str.c_str();
    write(new_socket , hello , strlen(hello));
    close(new_socket);
    }
    return 0;
}