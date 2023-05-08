

#include <string>
#include <sstream>
#include <vector>
#include "../utils/Option.h"
#include "../utils/utils.h"
#include "../http/http.h"

Option<std::string> find_key_value_line(std::string &str, const std::string &key, bool remove_line)
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

std::string get_value_from_line(const std::string &line)
{
	std::size_t pos = line.find_first_of(": ");
	if (pos == std::string::npos)
		throw std::runtime_error("Invalid config file, missing ':'");
	if (line.size() == pos + 2)
		throw std::runtime_error("Invalid config file, missing value after ':'");
	return line.substr(pos + 2);
}

Option<short> parse_methods(std::string &str)
{
	Option<std::string> line = find_key_value_line(str, "methods", true);
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
		else if (*it == "PUT" || *it == "OPTIONS" || *it == "TRACE" || *it == "PATCH" || *it == "HEAD" ||
				 *it == "CONNECT")
			throw std::runtime_error("Invalid config file, " + *it + " method is not implemented");
		else
			throw std::runtime_error("Invalid config file, '" + *it + "' is not a valid method");
	}
	return Option<short>::Some(methods);
}

std::string unpad_from_left(const std::string &str, size_t padding, bool throw_if_something)
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