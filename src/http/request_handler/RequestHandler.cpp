#include "RequestHandler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include "../../cgi_executor/CgiExecutor.h"

RequestHandler::RequestHandler(const Config *config) : config_(config)
{
}

HttpResponse RequestHandler::handle_redirection(const Route* route) {
    HttpResponse 	response;
    std::string		response_url;
	std::string		redirection;

	redirection = route->redirection.unwrap();
	if (redirection.at(0) == '/')
		response_url = "http://localhost:8080" + route->redirection.unwrap();
	else
		response_url = "http://" + route->redirection.unwrap();
    response.set_version("HTTP/1.1");
    response.set_status(302, "Found");
    response.add_header("Location", response_url);
	std::cout << "Redirection URL: " << response_url << std::endl;
    return response;
}

Route * RequestHandler::getRouteOrThrowResponse(const HttpRequest &request)
{
    Option<HttpResponse> res = checkRequestValidity(request);
    if (res.isSome())
        throw res.unwrap();
    Route *route = find_route(request.get_path());
    if (route == NULL)
        throw handle_error_static(404, "Not Found (route)", config_);
    if (!is_method_allowed(route, request))
        throw handle_error_static(405, "Method Not Allowed", config_);
    if (check_path(request.get_path()))
        throw handle_error_static(403, "Forbidden", config_);
    if (route->redirection.isSome())
        throw handle_redirection(route);
    return route;
}

Option<HttpResponse> RequestHandler::checkRequestValidity(const HttpRequest& request) {
        std::istringstream  request_stream(request.get_raw());
        std::string         method;
        std::string         uri;
        std::string         http_version;

        // Parse the request line
        if (!(request_stream >> method >> uri >> http_version)) {
            return Option<HttpResponse>::Some(handle_error_static(400, "Bad Request", config_));
        }

        // Check if the method is valid
        if (method != "GET" && method != "POST" && method != "DELETE" && method != "PUT") {
            return Option<HttpResponse>::Some(handle_error_static(501, "Not Implemented", config_));
        }

        // Check if the HTTP version is valid
        if (http_version != "HTTP/1.1") {
            return Option<HttpResponse>::Some(handle_error_static(505, "HTTP Version Not Supported", config_));
        }

        // Check headers
        std::string header_line;
        while (std::getline(request_stream, header_line) && header_line != "\r") {
            size_t colon_pos = header_line.find(":");
            if (colon_pos == std::string::npos) {
                // Each header must contain a colon
                return Option<HttpResponse>::Some(handle_error_static(400, "Bad Request", config_));
            }

            std::string header_name = header_line.substr(0, colon_pos);
            std::string header_value = header_line.substr(colon_pos + 1);

            // Remove leading and trailing whitespace from the header value
            size_t first_not_space = header_value.find_first_not_of(" ");
            size_t last_not_space = header_value.find_last_not_of(" ");
            header_value = header_value.substr(first_not_space, last_not_space - first_not_space + 1);

            if (header_name.empty() || header_value.empty()) {
                // Both header name and value must be non-empty
                 return Option<HttpResponse>::Some(handle_error_static(400, "Bad Request", config_));
            }
        }

        // The request line and headers are well formed
        return Option<HttpResponse>::None();
    }

Route *RequestHandler::find_route(const std::string &requested_path) const
{
    Route *best_match = 0;
    std::string best_route;
    std::string modified_request_path = requested_path;

    // If requested_path doesn't end with a '/', append one
    if (!requested_path.empty() && *(requested_path.end() - 1) != '/')
        modified_request_path += '/';

    std::map<std::string, Route *>::const_iterator it;
    for (it = config_->routes.begin(); it != config_->routes.end(); ++it)
    {
        std::string route_path = it->first;

        // If route_path doesn't end with a '/', append one
        if (!route_path.empty() && *(route_path.end() - 1) != '/')
            route_path += '/';

        if (modified_request_path.find(route_path) == 0 && route_path.length() > best_route.length())
        {
            best_match = it->second;
            best_route = it->first;
        }
    }
    return best_match;
}

bool RequestHandler::is_method_allowed(const Route *route, const HttpRequest &request)
{
	if (route == NULL || route->methods.isNone())
	{
		return false;
	}

	short allowed_methods = route->methods.unwrap();
	return (allowed_methods & request.get_method()) != 0;
}

const  Config * RequestHandler::getConfig() const
{
    return config_;
}

static std::string create_error_html_static(int error_code, const std::string &error_phrase)
{
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

HttpResponse RequestHandler::handle_error_static(int error_code, const std::string &error_phrase, const Config *config)
{
	HttpResponse response;	
    
    response.set_status(error_code, error_phrase);
	response.add_header("Content-Type", "text/html");
    if (config == NULL)
    {
        // TODO : check default error page
        response.set_body(create_error_html_static(error_code, error_phrase));
        return response;
    }

	std::map<int, std::string>::const_iterator it = config->error_pages.find(error_code);
	if (it != config->error_pages.end())
	{
		// If an error page is found, use it as the body
		std::string file_path = it->second;
		std::ifstream file(file_path.c_str(), std::ios::in | std::ios::binary);
		if (file.is_open())
		{
			std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			response.set_body(content);
		}
		else
		{
			response.set_body(create_error_html_static(error_code, error_phrase));
		}
	}
	else
	{
		// If no error page is found, create a default error page
		response.set_body(create_error_html_static(error_code, error_phrase));
	}
	return response;
}
