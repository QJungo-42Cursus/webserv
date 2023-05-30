#include "RequestHandler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <sys/stat.h>
#include <dirent.h>
#include "../../cgi_executor/CgiExecutor.h"

bool is_path_dir(const std::string &path)
{
	struct stat s;
	if (stat(path.c_str(), &s) == 0)
	{
		return S_ISDIR(s.st_mode);
	}
	return false;
}

bool is_path_file(const std::string &path)
{
	struct stat s;
	if (stat(path.c_str(), &s) == 0)
	{
		return S_ISREG(s.st_mode);
	}
	return false;
}

std::string real_path(const Route &route, const HttpRequest &request)
{
	std::string requested_path;
	std::string route_path;
	std::string route_root;

	requested_path = request.get_path();
	requested_path.erase(requested_path.find_last_not_of("%20") + 1);
	route_path = route.name;
	route_root = route.root;

	if (route_root[route_root.size() - 1] != '/')
		route_root += '/';

	if (requested_path[requested_path.size() - 1] != '/')
		requested_path += '/';

	size_t pos = requested_path.find(route_path);

	if (pos != std::string::npos)
	{
		requested_path.replace(pos, route_path.length(), route_root);
	}

	if (requested_path[requested_path.size() - 1] == '/')
		requested_path.erase(requested_path.size() - 1);

	bool is_directory = is_path_dir(requested_path);

	if (requested_path[requested_path.size() - 1] != '/')
		requested_path += '/';
	return requested_path;
}

/// This will return a string containing the HTML code for a directory listing
std::string dir_listing(std::string const &path, std::string const &uri)
{
	std::cout << std::endl << "===================" << std::endl << std::endl;
	std::cout << "dir_listing: " << path << std::endl << "uri: " << uri << std::endl;

	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(path.c_str())) == NULL)
		throw std::runtime_error("Could not open directory");

	std::vector<std::string> files;
	while ((ent = readdir(dir)) != NULL)
		files.push_back(ent->d_name);
	std::sort(files.begin(), files.end());

	closedir(dir);

	std::string html = "<html><head><title>Index of ";
	html += path + "</title></head><body><h1>Index of " + uri + "</h1><hr>\n";
	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it)
	{
		std::string href;
		if (uri.rfind('/') == uri.length() - 1)
			href = uri + *it;
		else
			href = uri + "/" + *it;
		html += "<a href=\"" + href + "\">" + *it + "/</a><br>\n";
	}
	html += "<hr></body></html>";
	return html;
}

std::pair<std::string, Option<std::string> > get_path_and_file(const std::string &path)
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


std::string get_content_type(const std::string &path)
{
	std::string::size_type idx = path.rfind('.');
	if (idx != std::string::npos)
	{
		std::string extension = path.substr(idx + 1);
		if (extension == "html" || extension == "htm")
		{
			return "text/html";
		}
	}
	return "text/plain";
}

//HttpResponse parseCGIResponse(const std::string &cgi_response)
//{
//	HttpResponse response;
//
//	std::size_t body_start = cgi_response.find("\r\n\r\n");
//	if (body_start == std::string::npos)
//	{
//		response.set_body(cgi_response);
//	}
//	else
//	{
//		response.set_body(cgi_response.substr(body_start + 4));
//
//		// Processing headers
//		std::istringstream headers_stream(cgi_response.substr(0, body_start));
//		std::string header_line;
//
//		while (std::getline(headers_stream, header_line))
//		{
//			std::size_t separator_pos = header_line.find(":");
//			if (separator_pos != std::string::npos)
//			{
//				std::string header_name = header_line.substr(0, separator_pos);
//				std::string header_value = header_line.substr(separator_pos + 1);
//
//				if (header_name == "Status")
//				{
//					std::size_t status_end = header_value.find(" ");
//					std::string status_code = header_value.substr(1, status_end - 1);
//					std::string status_text = header_value.substr(status_end + 1);
//					response.set_status(std::stoi(status_code), status_text);
//				}
//				else
//				{
//					response.add_header(header_name, header_value.substr(1));
//				}
//			}
//		}
//	}
//	return response;
//}


static std::string getStatus(std::vector<std::string> &headers)
{
	for (std::vector<std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
	{
		if (it->find("Status:") == 0)
		{
			std::string status = it->substr(7);
			headers.erase(it);
			return status;
		}
	}
	return "200 OK";
}

HttpResponse parseCGIResponse(const std::string &cgiOutput)
{
	HttpResponse response;

	/// Split headers and body
	std::string::size_type separatorPos = cgiOutput.find("\r\n\r\n");
	if (separatorPos == std::string::npos)
	{
		// TODO: throw exception ou juste mode body ?
	}
	response.set_body(cgiOutput.substr(separatorPos + 4));

	/// Split headers
	std::string headersStr = cgiOutput.substr(0, separatorPos);
	std::vector<std::string> headers = split(headersStr, "\r\n");

	/// Get status
	std::string status = getStatus(headers);
	int status_code = std::atoi(status.substr(0, status.find(" ")).c_str());
	response.set_status(status_code, status.substr(status.find(" ") + 1));

	/// Add headers to response
	for (std::vector<std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
	{
		std::string::size_type separatorPosa = it->find(":");
		if (separatorPosa != std::string::npos)
		{
			std::string headerName = it->substr(0, separatorPosa);
			std::string headerValue = it->substr(separatorPosa + 1);
			response.add_header(headerName, headerValue.substr(1));
		}
	}
	return response;
}