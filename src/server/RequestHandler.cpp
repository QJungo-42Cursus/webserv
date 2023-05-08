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
    if (route == nullptr || route->methods.isNone()) {
        return false;
    }

    short allowed_methods = route->methods.unwrap();
    return (allowed_methods & request.get_method()) != 0;
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
    if (route == nullptr) {
        response.set_status(404, "Not Found");
        return response;
    }

    if (!is_method_allowed(route, request)) {
        response.set_status(405, "Method Not Allowed");
        return response;
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
        response.set_status(404, "Not Found");
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
    if (route == nullptr) {
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
    if (route == nullptr) {
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
