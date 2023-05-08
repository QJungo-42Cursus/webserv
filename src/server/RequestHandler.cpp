#include "RequestHandler.h"
#include <iostream>
#include <fstream>
#include <sstream>

std::string get_content_type(const std::string& path) {
    std::string::size_type idx = path.rfind('.');
    if (idx != std::string::npos) {
        std::string extension = path.substr(idx + 1);
        if (extension == "html" || extension == "htm") {
            return "text/html";
        }
    }
    return "text/plain";
}

HttpResponse GetRequestHandler::handle_request(const HttpRequest& request) {
    // Handle GET request
    std::string path = "website" + request.get_path();
    std::string file_content;
    std::string content_type;

    // Check if the requested file exists
    std::ifstream requested_file(path.c_str());
    if (requested_file.is_open() && path != "website/") {
        // Read the requested file content
        std::stringstream file_stream;
        file_stream << requested_file.rdbuf();
        file_content = file_stream.str();
        requested_file.close();
        content_type = get_content_type(path);
    } else {
        // Open the default file if the requested file doesn't exist
        std::ifstream default_file("website/default.html");
        if (default_file.is_open()) {
            std::stringstream file_stream;
            file_stream << default_file.rdbuf();
            file_content = file_stream.str();
            default_file.close();
            content_type = get_content_type("website/default.html");
        } else {
            file_content = "Error: Default file not found.";
            content_type = "text/plain";
        }
    }

    // Process GET request based on the path and any other necessary data from the request
    std::cout << "Handling GET request for path: " << path << std::endl;

    // Create an HttpResponse object and return it
    std::map<std::string, std::string> headers;
    headers["Content-Type"] = content_type;
    return HttpResponse(200, "OK", headers, file_content);
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
