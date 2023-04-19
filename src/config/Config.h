#ifndef CONFIG_H
#define CONFIG_H
#include "../utils/Option.h"
#include <map>
#include <vector>
#include <string>

enum HttpMethods
{
	GET,
	POST,
	PUT,
	DELETE,
	HEAD,
	OPTIONS,
	TRACE,
	CONNECT
};

struct Cgi
{
};

struct Route
{
	/// list of the methods accepted by the route
	std::map<HttpMethods, bool> accepted_methods;

	/// http redirection
	Option<std::string> redirect;

	/// c'est le path de la route sur le serveur
	std::string path;

	/// if the directory listing is enabled
	bool dir_listing;

	/// file to serve if path is a dir
	std::string path_if_dir;

	/// The cgi configuration
	Option<Cgi> cgi;
};

class Config
{
private:
	int port;
	int host;
	std::string server_name; // optional
	std::string path_to_error_page;
	int max_client_body_size;
	std::vector<Route> routes;
};

#endif