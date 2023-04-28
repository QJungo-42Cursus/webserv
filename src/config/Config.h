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

	void log() const;

	~Config();

	Option<std::string> server_name;
	Option<int> port;
	Option<std::string> client_max_body_size;
	Option<short> methods;
	std::map<int, std::string> error_pages;
	std::map<std::string, Route *> routes;
private:
	Config()
	{}


};

#endif