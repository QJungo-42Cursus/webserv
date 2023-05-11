#ifndef CONFIG_H
#define CONFIG_H

#include "../http/Methods.h"
#include "Route.h"
#include "../utils/Option.h"
#include <map>
#include <vector>
#include <string>
#include "../utils/utils.h"
#include "yaml_helper.h"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <set>
#include <unistd.h>

class Route;

class Config
{
public:
	static std::vector<Config *> parse_servers(const std::string &path);

	static Config *parse(std::string &server_config);

	void log() const;

	struct Default
	{
		static const int PORT;
		static const unsigned int CLIENT_MAX_BODY_SIZE;
	};

	~Config();

	Option<std::string> server_name;
	Option<int> port;
	Option<unsigned int> client_max_body_size;
	std::map<int, std::string> error_pages;
	std::map<std::string, Route *> routes;
private:
	Config();
};

#endif