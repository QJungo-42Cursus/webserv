#ifndef CONFIG_H
#define CONFIG_H
#include "../utils/Option.h"
#include <map>
#include <vector>
#include <string>
#include "../http/Methods.h"

struct Cgi
{
	std::string file_extension;
	std::string cgi_path;
};

struct Route
{
	/// bit flag of the methods accepted by the route
	short methods; // = Http::Methods::GET;

	/// http redirection
	Option<std::string> redirection; // = Option<std::string>::None();

	/// c'est le path de la route sur le serveur
	std::string root;

	/// if the directory listing is enabled
	bool repertory_listing;

	/// file to serve if path is a dir
	Option<std::string> index; // = Option<std::string>::None();

	/// The cgi configuration
	Option<Cgi> cgi;

	/// The upload directory (disabled if None)
	Option<std::string> upload_directory; // = Option<std::string>::None();
};

class Config
{
public:
	static std::vector<Config *> parse_servers(const std::string &path);
	static Config *parse(std::string &server_config);

private:
	Config() {}
	Option<std::string> _server_name;
	Option<int> _port;
	Option<std::string> _client_max_body_size;
	Option<short> _methods;
	std::map<int, std::string> _error_pages;
	std::vector<Route> _routes;
};

#endif