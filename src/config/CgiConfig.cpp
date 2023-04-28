#include <vector>
#include "CgiConfig.h"
#include "../utils/utils.h"

CgiConfig CgiConfig::parse(std::string &cgi_config)
{
	std::vector<std::string> words = split(cgi_config, ' ');
	if (words.size() != 2)
		throw std::runtime_error("invalid config file: cgi: <file_extension> <cgi_path>");
	if (words[0].empty() || words[1].empty())
		throw std::runtime_error("invalid config file: cgi: <file_extension> <cgi_path>");
	CgiConfig config;
	config.file_extension = words[0];
	config.cgi_path = words[1];
	return config;
}

std::ostream &operator<<(std::ostream &os, const CgiConfig &config)
{
	os << "CgiConfig(" << config.file_extension << ", " << config.cgi_path << ")";
	return os;
}
