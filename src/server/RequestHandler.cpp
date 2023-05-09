#include "RequestHandler.h"
#include <iostream>
#include <fstream>
#include <sstream>

RequestHandler::RequestHandler(const Config* config) : config_(config) {}

Route* RequestHandler::find_route(const std::string& requested_path) const{
    std::map<std::string, Route*>::const_iterator it;
    for (it = config_->routes.begin(); it != config_->routes.end(); ++it) {
        if (requested_path.find(it->first) == 0) {
            return it->second;
        }
    }
    return 0;
}


bool RequestHandler::is_method_allowed(const Route* route, const HttpRequest& request) {
    if (route == NULL || route->methods.isNone()) {
        return false;
    }

    short allowed_methods = route->methods.unwrap();
    return (allowed_methods & request.get_method()) != 0;
}

std::string RequestHandler::create_error_html(int error_code, const std::string& error_phrase) const {
    std::stringstream html;
    html << "<!DOCTYPE html>"
         << "<html>"
         << "<head><title>Error Page</title></head>"
         << "<body>"
         << "<h1 style=\"text-align:center;\">" << error_code << " " << error_phrase << "</h1>"
         << "<p style=\"text-align:center;\">42_webserv</p>"
         << "</body>"
         << "</html>";
    return html.str();
}

HttpResponse RequestHandler::handle_error(int error_code, const std::string& error_phrase) {
    HttpResponse response;

    std::map<int, std::string>::const_iterator it = config_->error_pages.find(error_code);
    if (it != config_->error_pages.end()) {
        // If an error page is found, use it as the body
        std::string file_path = it->second;
        std::cout << file_path << std::endl;
        std::ifstream file(file_path.c_str(), std::ios::in | std::ios::binary);
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            response.set_body(content);
        } else {
            response.set_body(create_error_html(error_code, error_phrase));    
        } 
    } else {
        // If no error page is found, create a default error page
        response.set_body(create_error_html(error_code, error_phrase));
    }
    response.set_version("HTTP/1.1");
    response.set_status(error_code, error_phrase);
    response.add_header("Content-Type", "text/html");

    return response;
}

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

GetRequestHandler::GetRequestHandler(const Config* config) : RequestHandler(config) {}

HttpResponse GetRequestHandler::handle_request(const HttpRequest& request) {
    HttpResponse response;
    response.set_version(request.get_version());

    Route* route = find_route(request.get_path());
    if (route == NULL) {
        return handle_error(404, "Not Found");
    }

    if (!is_method_allowed(route, request)) {
        return handle_error(405, "Method Not Allowed");
    }

    std::string file_path;
    if (route->root.isSome()) {
        file_path = route->root.unwrap();
        bool is_directory = (request.get_path()[request.get_path().length() - 1] == '/');
        bool has_extension = (request.get_path().find('.') != std::string::npos);

        if (!has_extension && is_directory) {
            if (route->index.isSome()) {
                file_path += route->index.unwrap();
            }
        }
        else
        {
            file_path += request.get_path();
        }
    } else {
        file_path = request.get_path();
    }

    std::ifstream file(file_path.c_str(), std::ios::in | std::ios::binary);

    if (!file) {
        return handle_error(404, "Not Found");
    } else {
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        response.set_body(content);
        response.set_status(200, "OK");
        response.add_header("Content-Type", get_content_type(file_path));
    }
    return response;
}

PostRequestHandler::PostRequestHandler(const Config* config) : RequestHandler(config) {}

HttpResponse PostRequestHandler::handle_request(const HttpRequest& request) {
    HttpResponse response;
    response.set_version(request.get_version());

    Route* route = find_route(request.get_path());
    if (route == NULL) {
        response.set_status(404, "Not Found");
        return response;
    }

    if (!is_method_allowed(route, request)) {
        response.set_status(405, "Method Not Allowed");
        return response;
    }

    // Process the POST request according to your application's requirements.
    // For example, you can save the request body to a file or a database.

    response.set_status(200, "OK");
    return response;
}

DeleteRequestHandler::DeleteRequestHandler(const Config* config) : RequestHandler(config) {}

HttpResponse DeleteRequestHandler::handle_request(const HttpRequest& request) {
    HttpResponse response;
    response.set_version(request.get_version());

    Route* route = find_route(request.get_path());
    if (route == NULL) {
        response.set_status(404, "Not Found");
        return response;
    }

    if (!is_method_allowed(route, request)) {
        response.set_status(405, "Method Not Allowed");
        return response;
    }

    std::string file_path;
    if (route->root.isSome()) {
        file_path = route->root.unwrap() + request.get_path();
    } else {
        file_path = request.get_path();
    }

    if (std::remove(file_path.c_str()) != 0) {
        response.set_status(500, "Internal Server Error");
    } else {
        response.set_status(200, "OK");
    }

    return response;
}
