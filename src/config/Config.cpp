#include "Config.h"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <string>
#include <sstream>

static void open_file(std::ifstream &file, std::string filename)
{
	file.open(filename.c_str(), std::ios::in);
	if (!file.good())
		throw std::runtime_error("Can't open file " + filename);
	if (!file.is_open())
		throw std::runtime_error("Can't open file " + filename);
}

static void left_trim(std::string &str)
{
	str = str.erase(str.find_last_not_of(" \n\r\t") + 1);
}

static bool is_string_whitespace(const std::string &str)
{
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
	{
		if (!std::isspace(*it))
			return false;
	}
	return true;
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

static bool is_any_quote(const std::string &str)
{
	return str.find('\'') != std::string::npos || str.find('"') != std::string::npos;
}

static bool are_tirets_valid(const std::string &str)
{
	std::istringstream iss(str);
	for (std::string line; std::getline(iss, line);)
	{
		size_t pos = line.find_first_not_of(" ");
		if (pos != std::string::npos && line[pos] == '-')
		{
			if (line[pos + 1] != ' ')
				return false;
			if (!isprint(line[pos + 2]) || line[pos + 2] == ' ')
				return false;
		}
	}
	return true;
}

/// Remove the padding from the left of the string, at each line
static std::string unpad_from_left(const std::string &str, size_t padding)
{
	std::string result = "";
	std::istringstream iss(str);
	for (std::string line; std::getline(iss, line);)
	{
		if (line.size() > padding)
			result += line.substr(padding) + "\n";
		else
			result += "\n";
	}
	return result;
}


std::vector<Config> Config::parse(const std::string &path)
{
	/// Open the file
	std::ifstream file;
	open_file(file, path); // TODO: handle exception

	/// Get the file content without comments
	std::string file_content = get_file_content_whithout_comments(file);

	/// Close the file
	file.close();

	/// Check if the file is empty
	if (file_content.empty())
		throw std::runtime_error("Empty config file");
	
	/// Check if the file contains only valid characters
	if (is_any_quote(file_content))
		throw std::runtime_error("Invalid config file, invalid quotes");

	/// Check if the file starts with "servers"
	// TODO check if the file starts with "servers" + ":" + "\n" -> all keys: !
	if ("servers" != file_content.substr(0, 7))
		throw std::runtime_error("Invalid config file, missing 'servers' keyword at the beginning");
	file_content = file_content.erase(0, file_content.find_first_of("\n") + 1);

	/// Check if the file contains only valid tirets
	if (!are_tirets_valid(file_content))
		throw std::runtime_error("Invalid config file, invalid tirets");

	/// Split the file content into servers
	std::vector<std::string> servers;

	file_content = unpad_from_left(file_content, 4);

	while (true)
	{
		if (file_content.empty())
			break;
		if ("server" != file_content.substr(0, 6))
			throw std::runtime_error("Invalid config file, missing 'server' keyword at the beginning");
		file_content = file_content.erase(0, file_content.find_first_of("\n") + 1);
		size_t pos = file_content.find("server"); // TODO nonoteunoe
		servers.push_back(file_content.substr(0, pos));

		file_content = file_content.erase(0, pos);
	}

	for (std::vector<std::string>::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		std::cout << *it << std::endl << std::endl;
	}

	std::vector<Config> configs;

	std::cout << file_content;
	return configs;
}
