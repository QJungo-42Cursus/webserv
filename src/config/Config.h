#ifndef CONFIG_H
#define CONFIG_H

#include "../utils/Option.h"
#include <map>
#include <vector>
#include <string>
#include "../http/Methods.h"
#include "Route.h"

class Route;

class Config
{
public:
	static std::vector<Config *> parse_servers(const std::string &path);

	static Config *parse(std::string &server_config);

	~Config();
private:
	Config()
	{}


	Option<std::string> _server_name;
	Option<int> _port;
	Option<std::string> _client_max_body_size;
	Option<short> _methods;
	std::map<int, std::string> _error_pages;
	std::map<std::string, Route*> _routes;
};

#endif