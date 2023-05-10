#include "RequestHandler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <dirent.h>

RequestHandler::RequestHandler(const Config *config) : config_(config)
{}

Route *RequestHandler::find_route(const std::string &requested_path) const
{
	// TODO la question: est-ce que '/admin/user/id' fait parti de la route '/admin/' ?
	// 	Si oui, que faire si '/admin/' et '/admin/user/' sont tous les deux des routes ?
	std::map<std::string, Route *>::const_iterator it;
	for (it = config_->routes.begin(); it != config_->routes.end(); ++it)
	{
		std::size_t req_len = requested_path.length();
		if (req_len > it->first.length())
			continue;
		if (requested_path.find(it->first) == 0)
		{
			std::cout << "find_route: " << it->first <<
					  " for requested_path: " << requested_path <<
					  std::endl;
			return it->second;
		}
	}
	return 0;
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
	response.set_version("HTTP/1.1");
	response.set_status(error_code, error_phrase);
	response.add_header("Content-Type", "text/html");

	return response;
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

GetRequestHandler::GetRequestHandler(
		const Config *config) : RequestHandler(config)
{}


static bool is_path_dir(const std::string &path)
{
	struct stat s;
	if (stat(path.c_str(), &s) == 0)
	{
		return S_ISDIR(s.st_mode);
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
	HttpResponse response;
	// TODO: on veut pas crash si on ne supporte pas la version
	//  et mettre toujours la version du request dans le response, directement dans le constructeur ?
	response.set_version(request.get_version());

	std::cout << "====================" << std::endl;
	std::cout << "full path: " << request.get_path() << std::endl;
	std::pair<std::string, Option<std::string> > path_and_file = get_path_and_file(
			request.get_path()
	);
	std::cout << "path: " << path_and_file.first << std::endl;
	std::cout << "file: " << path_and_file.second << std::endl;

	Route *route = find_route(path_and_file.first);

	if (route == NULL)
	{
		// TODO si la route n'existe pas, on peut quand meme renvoyer un fichier avec le path par defaut
		// PWD + www + path ?
		return handle_error(404, "Not Found");
	}

	route->log();
	std::cout << "====================" << std::endl;

	if (!is_method_allowed(route, request))
	{
		return handle_error(405, "Method Not Allowed");
	}

	std::string file_path;
	if (route->root.isSome())
	{
		// TODO: je ne crois pas que le / a la fin ne signifie que c'est un dossier
		bool is_directory = (request.get_path()[request.get_path().length() - 1] == '/');
		is_directory = path_and_file.second.isNone() && is_path_dir(route->root.unwrap());
		if (path_and_file.second.isSome())
		{
			is_directory = is_path_dir(route->root.unwrap() + path_and_file.second.unwrap());
			if (is_directory)
				throw std::runtime_error(
						"TODO: the client ask for a directory, like it was a file. For path : " + route->root.unwrap() +
						path_and_file.second.unwrap());
		}

		// TODO: gerer s'il y a plusieurs points dans le path
		bool has_extension = (request.get_path().find('.') != std::string::npos);

		file_path = route->root.unwrap();
		if (!has_extension && is_directory)
		{
			if (route->repertory_listing)
			{
				response.set_body(dir_listing(route->root.unwrap(), request.get_path()));
				response.set_status(200, "OK");
				response.add_header("Content-Type", "text/html");
				return response;
			}
			else if (route->index.isSome())
			{
				file_path += route->index.unwrap();
			}
			else
			{
				throw std::runtime_error("TODO: no repertory listing and no index. But the path is a directory.");
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
	response.set_version(request.get_version());

	Route *route = find_route(request.get_path());
	if (route == NULL)
	{
		response.set_status(404, "Not Found");
		return response;
	}

	if (!is_method_allowed(route, request))
	{
		response.set_status(405, "Method Not Allowed");
		return response;
	}

	// Process the POST request according to your application's requirements.
	// For example, you can save the request body to a file or a database.

	response.set_status(200, "OK");
	return response;
}

DeleteRequestHandler::DeleteRequestHandler(
		const Config *config) : RequestHandler(config)
{}

HttpResponse DeleteRequestHandler::handle_request(const HttpRequest &request)
{
	HttpResponse response;
	response.set_version(request.get_version());

	Route *route = find_route(request.get_path());
	if (route == NULL)
	{
		response.set_status(404, "Not Found");
		return response;
	}

	if (!is_method_allowed(route, request))
	{
		response.set_status(405, "Method Not Allowed");
		return response;
	}

	std::string file_path;
	if (route->root.isSome())
	{
		file_path = route->root.unwrap() + request.get_path();
	}
	else
	{
		file_path = request.get_path();
	}

	if (std::remove(file_path.c_str()) != 0)
	{
		response.set_status(500, "Internal Server Error");
	}
	else
	{
		response.set_status(200, "OK");
	}

	return response;
}
