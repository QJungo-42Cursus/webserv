#ifndef ROUTE_H
#define ROUTE_H

#include <string>
#include "../utils/Option.h"
#include "Config.h"
#include "CgiConfig.h"

class Route
{
public:
	static Route *parse(std::string &route_config, std::string const &route_name);

	void log() const;

	~Route();

	struct DEFAULT
	{
		static const short METHODS;
		static const bool REPERTORY_LISTING;
		static const Option<std::string> INDEX;
	};
	Option<short> methods;
	Option<std::string> redirection;
	std::string root;
//	Option<std::string> root;
	bool repertory_listing;
	Option<std::string> index;
	Option<CgiConfig> cgi;
	Option<std::string> upload_directory;
    std::string name;
};


#endif //ROUTE_H
