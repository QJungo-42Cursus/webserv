#ifndef CGICONFIG_H
#define CGICONFIG_H

#include <string>

class CgiConfig
{
public:
	static CgiConfig parse(std::string &cgi_config);

	std::string file_extension;
	std::string cgi_path;

	void log() const;
};


#endif //CGICONFIG_H
