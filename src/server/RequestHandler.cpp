#include "RequestHandler.h"
#include <iostream>

HttpResponse GetRequestHandler::handle_request(const HttpRequest& request) {
    // Handle GET request
    std::string path = request.get_path();
    // Process GET request based on the path and any other necessary data from the request
    std::cout << "Handling GET request for path: " << path << std::endl;

    // Create an HttpResponse object and return it
    std::map<std::string, std::string> headers;
    headers["Content-Type"] = "text/plain";
    return HttpResponse(200, "OK", headers, "GET response");
}

HttpResponse PostRequestHandler::handle_request(const HttpRequest& request) {
    // Handle POST request
    std::string path = request.get_path();
    std::string body = request.get_body();
    // Process POST request based on the path, body, and any other necessary data from the request
    std::cout << "Handling POST request for path: " << path << " with body: " << body << std::endl;

    // Create an HttpResponse object and return it
    std::map<std::string, std::string> headers;
    headers["Content-Type"] = "text/plain";
    return HttpResponse(200, "OK", headers, "POST response");
}

HttpResponse DeleteRequestHandler::handle_request(const HttpRequest& request) {
    // Handle DELETE request
    std::string path = request.get_path();
    // Process DELETE request based on the path and any other necessary data from the request
    std::cout << "Handling DELETE request for path: " << path << std::endl;

    // Create an HttpResponse object and return it
    std::map<std::string, std::string> headers;
    headers["Content-Type"] = "text/plain";
    return HttpResponse(200, "OK", headers, "DELETE response");
}
