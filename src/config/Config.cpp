#include "Config.h"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <string>
#include <sstream>
#include "../utils/utils.h"

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

std::size_t find_where_its_first_line_of_word(const std::string &str, const std::string &word)
{
	std::size_t pos = str.find(word);
	if (pos == std::string::npos)
		return std::string::npos;
	if (pos != 0 && str[pos - 1] != '\n')
		return std::string::npos;
	return pos;
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

	// Check if the file contains only valid keywords + column (key: value)

	/// Check if the file starts with "servers"
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
		std::string first_line = file_content.substr(0, file_content.find_first_of("\n"));
		std::cout << "first_line: " << first_line << std::endl;
		std::vector<std::string> splitted = split(first_line, ':');
		if ("server" != splitted[0])
			throw std::runtime_error("Invalid config file, '" + splitted[0] + "' is not a valid keyword");
		if (splitted.size() != 1)
			throw std::runtime_error("Invalid config file, '" + splitted[0] + "' keyword must be alone on its line");
		file_content.erase(0, file_content.find_first_of("\n") + 1);

		std::size_t pos = find_next_non_whitespace_linestart(file_content);
		// file_content.find("server:");
		servers.push_back(file_content.substr(0, pos));
		if (pos != std::string::npos)
		{
		}
		else
		{
			// throw std::runtime_error("Invalid config file, missing 'server' keyword at the end");
		}
		file_content = file_content.erase(0, pos);
	}

	for (std::vector<std::string>::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		std::cout << *it
				  << std::endl
				  << std::endl
				  << std::endl
				  << std::endl;
	}

	std::vector<Config> configs;

	return configs;
}
