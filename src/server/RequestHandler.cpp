#include "RequestHandler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <dirent.h>

RequestHandler::RequestHandler(const Config *config) : config_(config)
{
}

Option<HttpResponse> RequestHandler::parse(const HttpRequest& request) {
        std::istringstream  request_stream(request.get_raw());
        std::string         method;
        std::string         uri;
        std::string         http_version;

        // Parse the request line
        if (!(request_stream >> method >> uri >> http_version)) {
            return Option<HttpResponse>::Some(handle_error(400, "Bad Request"));
        }

        // Check if the method is valid
        if (method != "GET" && method != "POST" && method != "DELETE") {
            return Option<HttpResponse>::Some(handle_error(501, "Not Implemented"));
        }

        // Check if the HTTP version is valid
        if (http_version != "HTTP/1.1") {
            return Option<HttpResponse>::Some(handle_error(505, "HTTP Version Not Supported"));
        }

        // Check headers
        std::string header_line;
        while (std::getline(request_stream, header_line) && header_line != "\r") {
            size_t colon_pos = header_line.find(":");
            if (colon_pos == std::string::npos) {
                // Each header must contain a colon
                return Option<HttpResponse>::Some(handle_error(400, "Bad Request"));
            }

            std::string header_name = header_line.substr(0, colon_pos);
            std::string header_value = header_line.substr(colon_pos + 1);

            // Remove leading and trailing whitespace from the header value
            size_t first_not_space = header_value.find_first_not_of(" ");
            size_t last_not_space = header_value.find_last_not_of(" ");
            header_value = header_value.substr(first_not_space, last_not_space - first_not_space + 1);

            if (header_name.empty() || header_value.empty()) {
                // Both header name and value must be non-empty
                 return Option<HttpResponse>::Some(handle_error(400, "Bad Request"));
            }
        }

        // The request line and headers are well formed
        return Option<HttpResponse>::None();
    }
/*
Route *RequestHandler::find_route(const std::string &requested_path) const
{
    std::map<std::string, Route *>::const_iterator it;
    Route* closest_route = NULL;
    size_t closest_match_length = 0;

    for (it = config_->routes.begin(); it != config_->routes.end(); ++it)
    {
        std::size_t route_len = it->first.length();
        if (requested_path.find(it->first) == 0 && route_len > closest_match_length)
        {
            closest_route = it->second;
            closest_match_length = route_len;
        }
    }
    return closest_route;
}*/

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

std::string RequestHandler::create_error_html(int error_code, const std::string &error_phrase) const
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

HttpResponse RequestHandler::handle_error(int error_code, const std::string &error_phrase)
{
	HttpResponse response;

	std::map<int, std::string>::const_iterator it = config_->error_pages.find(error_code);
	if (it != config_->error_pages.end())
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
			response.set_body(create_error_html(error_code, error_phrase));
		}
	}
	else
	{
		// If no error page is found, create a default error page
		response.set_body(create_error_html(error_code, error_phrase));
	}
	response.set_status(error_code, error_phrase);
	response.add_header("Content-Type", "text/html");

	return response;
}

std::string get_content_type(const std::string &path) {
	std::string::size_type idx = path.rfind('.');
	if (idx != std::string::npos) {
		std::string extension = path.substr(idx + 1);
		if (extension == "html" || extension == "htm") {
			return "text/html";
		}
	}
	return "text/plain";
}

GetRequestHandler::GetRequestHandler(
		const Config *config) : RequestHandler(config)
{}


static bool is_path_dir(const std::string &path) {
	struct stat s;
	if (stat(path.c_str(), &s) == 0) {
		return S_ISDIR(s.st_mode);
	}
	return false;
}

bool is_path_file(const std::string& path) {
    struct stat s;   
    if(stat(path.c_str(), &s) == 0) {
    	return S_ISREG(s.st_mode);
	} 
    return false;
}

/// This will return a string containing the HTML code for a directory listing
std::string dir_listing(std::string const &path, std::string const &uri)
{
	std::vector<std::string> files;
	files.push_back("./");
	files.push_back("../");

	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(path.c_str())) == NULL)
		throw std::runtime_error("Could not open directory");

	while ((ent = readdir(dir)) != NULL)
	{
		if (ent->d_name[0] != '.' && ent->d_name != "..")
			files.push_back(ent->d_name);
	}
	closedir(dir);

	std::string html =
			"<html><head><title>Index of " + path + "</title></head><body><h1>Index of " + uri + "</h1><hr>\n";
	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it)
	{
		html += "<a href=\"" + *it + "\">" + *it + "</a><br>\n";
	}
	html += "<hr></body></html>";
	return html;
}

static std::pair<std::string, Option<std::string> > get_path_and_file(const std::string &path)
{
	std::string::size_type idx = path.rfind('/');
	if (idx != std::string::npos)
	{
		std::string file = path.substr(idx + 1);
		std::string n_path = path.substr(0, idx + 1);
		Option<std::string> opt_file = Option<std::string>::Some(file);
		if (file.empty())
			opt_file = Option<std::string>::None();
		return std::make_pair(n_path, opt_file);
	}
	return std::make_pair(path, Option<std::string>::None());
}

HttpResponse GetRequestHandler::handle_request(const HttpRequest &request)
{
	Option<HttpResponse> res = parse(request);
	if (res.isSome()) {
		return (res.unwrap());
	}
	HttpResponse response;

	std::pair<std::string, Option<std::string> > path_and_file = get_path_and_file(
			request.get_path()
	);

	Route *route = find_route(path_and_file.first);

	if (route == NULL) {
		return handle_error(404, "Not Found");
	}

	if (!is_method_allowed(route, request)) {
		return handle_error(405, "Method Not Allowed");
	}

	std::string file_path;

	if (route->root.isSome())
	{
		file_path = route->root.unwrap();
		/*
		if (file_path.back() == '/')
			file_path.erase(file_path.size() - 1);
			*/
		bool is_file = is_path_file(file_path + request.get_path());
		bool is_directory = is_path_dir(file_path + request.get_path());
		if (is_directory && *(request.get_path().end() - 1) != '/') {
			route = find_route(request.get_path().append("/"));
			file_path = route->root.unwrap();
			/*
			if (file_path.back() == '/')
				file_path.erase(file_path.size() - 1);
				*/
		}
		std::cout << "Dir: " << is_directory << std::endl;
		std::cout << "file: " << is_file << std::endl;
		if (!is_file && !is_directory)
			return handle_error(404, "Not Found");
		if (!is_file && is_directory)
		{
			if (route->repertory_listing) {
				file_path += request.get_path();
				std::cout << "Listing path: " << file_path << std::endl;
				response.set_body(dir_listing(file_path, request.get_path()));
				response.set_status(200, "OK");
				response.add_header("Content-Type", "text/html");
				return response;
			}
			else if (route->index.isSome()) {
				file_path += route->index.unwrap();
				std::cout << "Index path: " << std::endl;
			}
			else {
				return handle_error(403, "Forbidden");
			}
		}
		else
		{
			file_path += request.get_path();
		}
	}
	else
	{
		// TODO: mettre PWD + www + request.get_path() par defaut ?
		file_path = request.get_path();
	}

	std::ifstream file(file_path.c_str(), std::ios::in | std::ios::binary);

	if (!file)
	{
		return handle_error(404, "Not Found");
	}
	else
	{
		std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		response.set_body(content);
		response.set_status(200, "OK");
		response.add_header("Content-Type", get_content_type(file_path));
	}
	return response;
}

PostRequestHandler::PostRequestHandler(
		const Config *config) : RequestHandler(config)
{}

HttpResponse PostRequestHandler::handle_request(const HttpRequest &request)
{
	HttpResponse response;

	Route *route = find_route(request.get_path());
	if (route == NULL) {
		return handle_error(404, "Not Found");
	}

	if (!is_method_allowed(route, request)) {
		return handle_error(405, "Method Not Allowed");
	}

	// TODO: POST HANDLER
	response.set_status(200, "OK");
	return response;
}