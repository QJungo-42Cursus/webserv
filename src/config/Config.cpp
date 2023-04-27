#include "Config.h"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <string>
#include <cstdlib>
#include <sstream>
#include "../utils/utils.h"

static void open_file_or_throw(std::ifstream &file, std::string filename)
{
	file.open(filename.c_str(), std::ios::in);
	if (!file.good() || !file.is_open())
		throw std::runtime_error("Can't open file " + filename);
}

static void left_trim(std::string &str)
{
	str = str.erase(str.find_last_not_of(" \n") + 1);
}

static bool is_string_whitespace(const std::string &str)
{
	return str.find_first_not_of(" \n") == std::string::npos;
}

static std::string get_file_content_whithout_comments(std::ifstream &file)
{
	std::string file_content = "";
	std::string line;
	while (std::getline(file, line))
	{
		std::size_t comment_pos = line.find('#');
		if (comment_pos != std::string::npos)
		{
			line = line.substr(0, comment_pos);
			left_trim(line);
		}
		if (line.empty() || is_string_whitespace(line))
			continue;
		file_content += line + "\n";
	}
	return file_content;
}

static void throw_if_any_invalid_char(const std::string &str)
{
	// TODO: check if there is any OTHER ? invalid char
	if (str.find_first_of("'\"\t\r") != std::string::npos)
		throw std::runtime_error("Invalid config file, invalid character");
}

static void throw_if_invalid_tiret(const std::string &str)
{
	std::istringstream iss(str);
	for (std::string line; std::getline(iss, line);)
	{
		size_t pos = line.find_first_not_of(" ");
		if (pos != std::string::npos && line[pos] == '-')
		{
			if (line[pos + 1] != ' ')
				throw std::runtime_error("Invalid config file, need space after '-'");
			if (!isprint(line[pos + 2]) || line[pos + 2] == ' ')
				throw std::runtime_error("Invalid config file, need only one space after '-'");
		}
	}
}

static std::string KNOWN_KEYWORD[] = {
	"server",
	"servers",
	"port",
	"error_pages",
	"server_name",
	"methods",
	"root",
	"routes",
	"index",
	"redirection",
	"repertory_listing",
	"cgi",
	"upload",
	"client_max_body_size",
};

/// Check if the file contains only valid keywords + column (key: value)
static void throw_if_invalid_keyword(const std::string &str)
{
	std::istringstream iss(str);
	for (std::string line; std::getline(iss, line);)
	{
		std::size_t start = line.find_first_not_of(" \n");
		if (start % 2 != 0)
			throw std::runtime_error("Invalid config file, bad indentation");
		std::vector<std::string> splitted = split(line.substr(start), ':');
		if (splitted.size() == 0)
			throw std::runtime_error("Invalid config file");
		if (splitted.size() > 2)
			throw std::runtime_error("Invalid config file, too many ':'");
		bool found = false;
		for (std::size_t i = 0; i < sizeof(KNOWN_KEYWORD) / sizeof(KNOWN_KEYWORD[0]); ++i)
		{
			if (splitted[0].substr(0, 2) == "- ")
			{
				found = true;
				break;
			}
			else if (splitted[0] == KNOWN_KEYWORD[i])
			{
				found = true;
				break;
			}
		}
		if (!found)
			throw std::runtime_error("Invalid config file, unknown keyword '" + splitted[0] + "'");

		if ("- " != splitted[0].substr(0, 2) && splitted[0].find_first_of(" \n") != std::string::npos)
			throw std::runtime_error("Invalid config file, need space after '-' or ':'");
		if (splitted.size() == 2)
		{
			if (splitted[1][0] != ' ')
				throw std::runtime_error("Invalid config file, need space after ':'");
			if (splitted[1][1] == ' ')
				throw std::runtime_error("Invalid config file, need only one space after ':'");
		}
	}
}

static std::string unpad_from_left(const std::string &str, size_t padding, bool throw_if_something)
{
	std::string result = "";
	std::istringstream iss(str);
	for (std::string line; std::getline(iss, line);)
	{
		if (line.size() > padding)
			result += line.substr(padding) + "\n";
		else if (throw_if_something)
			throw std::runtime_error("Invalid config file, empty line");
		else
			result += "\n";
	}
	return result;
}

static std::size_t find_next_non_whitespace_linestart(const std::string &str)
{
	std::size_t pos = 0;

	while (true)
	{
		if (str[pos] != ' ' && str[pos] != '\n')
			return pos;
		pos = str.substr(pos).find("\n") + pos;
		if (pos == std::string::npos)
			return std::string::npos;
		pos++;
	}
	return pos;
}

std::vector<Config *> Config::parse_servers(const std::string &path)
{
	/// Get the file content without comments
	std::ifstream file;
	open_file_or_throw(file, path);
	std::string file_content = get_file_content_whithout_comments(file);
	file.close();

	/// Check file validity
	if (file_content.empty())
		throw std::runtime_error("Empty config file");
	throw_if_any_invalid_char(file_content);
	throw_if_invalid_keyword(file_content);
	throw_if_invalid_tiret(file_content);

	/// Check if the file starts with "servers"
	if ("servers" != file_content.substr(0, 7))
		throw std::runtime_error("Invalid config file, missing 'servers' keyword at the beginning");
	file_content = file_content.erase(0, file_content.find_first_of("\n") + 1);

	/// Split the file content into servers
	std::vector<std::string> servers;
	file_content = unpad_from_left(file_content, 2, true);
	file_content = unpad_from_left(file_content, 2, false); // don't check because tiret
	while (true)
	{
		if (file_content.empty())
			break;
		std::string first_line = file_content.substr(0, file_content.find_first_of("\n"));
		std::vector<std::string> splitted = split(first_line, ':');
		if ("server" != splitted[0])
			throw std::runtime_error("Invalid config file, '" + splitted[0] + "' is not a valid keyword");
		if (splitted.size() != 1)
			throw std::runtime_error("Invalid config file, '" + splitted[0] + "' keyword must be alone on its line");
		file_content.erase(0, file_content.find_first_of("\n") + 1);
		std::size_t pos = find_next_non_whitespace_linestart(file_content);
		std::string server_content = file_content.substr(0, pos);
		server_content = unpad_from_left(server_content, 2, true);
		servers.push_back(server_content);
		file_content = file_content.erase(0, pos);
	}

	/// Parse each server
	std::vector<Config *> configs;
	for (std::vector<std::string>::iterator it = servers.begin(); it != servers.end(); ++it)
		configs.push_back(parse(*it));
	return configs;
}

static Option<std::string> find_keyvalue_line(std::string &str, const std::string &key, bool remove_line)
{
	std::istringstream iss(str);
	for (std::string line; std::getline(iss, line);)
	{
		if (line.substr(0, key.size()) == key)
		{
			if (remove_line)
				str.erase(str.find(line), line.size() + 1);
			return Option<std::string>::Some(line);
		}
	}
	return Option<std::string>::None();
}

static std::string get_value_from_line(const std::string &line)
{
	std::size_t pos = line.find_first_of(": ");
	if (pos == std::string::npos)
		throw std::runtime_error("Invalid config file, missing ':'");
	if (line.size() == pos + 2)
		throw std::runtime_error("Invalid config file, missing value after ':'");
	return line.substr(pos + 2);
}

static Option<short> parse_methods(std::string &str)
{
	Option<std::string> line = find_keyvalue_line(str, "methods", true);
	if (line.isNone())
		return Option<short>::None();
	std::string value = get_value_from_line(line.unwrap());

	short methods = 0;
	std::vector<std::string> splitted = split(value, ' ');
	for (std::vector<std::string>::iterator it = splitted.begin(); it != splitted.end(); ++it)
	{
		if (*it == "GET")
			methods |= Http::Methods::GET;
		else if (*it == "POST")
			methods |= Http::Methods::POST;
		else if (*it == "DELETE")
			methods |= Http::Methods::DELETE;
		else if (*it == "PUT" || *it == "OPTIONS" || *it == "TRACE" || *it == "PATCH" || *it == "HEAD" || *it == "CONNECT")
			throw std::runtime_error("Invalid config file, " + *it + " method is not implemented");
		else
			throw std::runtime_error("Invalid config file, '" + *it + "' is not a valid method");
	}
	return Option<short>::Some(methods);
}

static Option<std::string> get_list_content(std::string &str, std::string key)
{
	std::string lines = "";
	bool found = false;
	std::istringstream iss(str);
	for (std::string line; std::getline(iss, line);)
	{
		if (found)
		{
			if (line.substr(0, 2) == "  ")
			{
				str.erase(str.find(line), line.size() + 1);
				lines += line + "\n";
			}
			else
				break;
		}
		else if (line.substr(0, key.size()) == key)
		{
			found = true;
			str.erase(str.find(line), line.size() + 1);
		}
	}
	if (!found)
		return Option<std::string>::None();
	lines = unpad_from_left(lines, 2, true);
	return Option<std::string>::Some(lines);
}

static std::map<int, std::string> parse_error_pages(std::string &str)
{
	std::map<int, std::string> map;
	Option<std::string> m_lines = get_list_content(str, "error_pages");
	if (m_lines.isNone())
		return map;
	std::string lines = m_lines.unwrap();
	std::vector<std::string> splitted = split(lines, '\n');
	for (std::vector<std::string>::iterator it = splitted.begin(); it != splitted.end(); ++it)
	{
		if ("- " != it->substr(0, 2))
			throw std::runtime_error("Invalid config file, an error_page must be in the form '- <error_code> <path>'");
		std::string line = it->substr(2);
		std::vector<std::string> splitted = split(line, ':');
		if (splitted.size() != 2)
			throw std::runtime_error("Invalid config file, an error_page must be in the form '- <error_code> <path>'");
		if (splitted[0].size() != 3)
			throw std::runtime_error("Invalid config file, an error_code must be 3 digits long");
		if (splitted[1][0] != ' ')
			throw std::runtime_error("Invalid config file, an error_page must be in the form '- <error_code> <path>'");
		map[std::atoi(splitted[0].c_str())] = splitted[1].substr(1);
	}
	return map;
}

static std::map<std::string, Route> parse_routes(std::string &str)
{
	std::map<std::string, Route> map;
	Option<std::string> m_lines = get_list_content(str, "routes");
	if (m_lines.isNone())
		return map;
	std::string lines = m_lines.unwrap();
	std::istringstream iss(lines);
	std::string line;
	for (std::string line; std::getline(iss, line);)
	{
		if ("- " != line.substr(0, 2))
			throw std::runtime_error("Invalid config file, a route must be in the form '- <path> \\n <infos>'");
		std::string route = line.substr(2);
		std::vector<std::string> splitted = split(route, ':');
		if (splitted.size() != 1)
			throw std::runtime_error("Invalid config file, a route must be in the form '- <path> \\n <infos>'");
		std::string route_path = splitted[0];
		std::string route_infos = "";
		// for (std::string line; std::getline(iss, line);)
		// {
		// 	if (line.substr(0, 4) == "    ")
		// 	{
		// 		route_infos += line + "\n";
		// 		str.erase(str.find(line), line.size() + 1);
		// 	}
		// 	else
		// 		break;
		// }

		// std::cout << "route_infos: " << route_infos << std::endl;
		// map[splitted[0]] = Route(splitted[1]);
	}

	return map;
}

Config *Config::parse(std::string &server_config)
{
	Config *config = new Config();

	{
		Option<std::string> line = find_keyvalue_line(server_config, "server_name", true);
		if (line.isSome())
			config->_server_name = Option<std::string>::Some(get_value_from_line(line.unwrap()));
	}
	{

		Option<std::string> line = find_keyvalue_line(server_config, "client_max_body_size", true);
		if (line.isSome())
			config->_client_max_body_size = Option<std::string>::Some(get_value_from_line(line.unwrap()));
	}
	{
		Option<std::string> line = find_keyvalue_line(server_config, "port", true);
		if (line.isSome())
			config->_port = Option<int>::Some(std::atoi(get_value_from_line(line.unwrap()).c_str()));
	}
	config->_methods = parse_methods(server_config);
	config->_error_pages = parse_error_pages(server_config);
	config->_routes = parse_routes(server_config);
	return config;
}