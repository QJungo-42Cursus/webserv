#ifndef ROUTE_H
#define ROUTE_H


#include <string>
#include "../utils/Option.h"
#include "Config.h"
#include "CgiConfig.h"

class Route
{
public:
	static Route *parse(std::string &route_config);

	void log() const;

	struct DEFAULT
	{
		static const short METHODS;
		static const bool REPERTORY_LISTING;
		static const Option<std::string> INDEX;
		static const Option<std::string> ROOT;
	};


	~Route()
	{}

public:
	Option<short> methods;
	Option<std::string> redirection;
	Option<std::string> root;
	bool repertory_listing;
	Option<std::string> index;
	Option<CgiConfig> cgi;
	Option<std::string> upload_directory;
};


#endif //ROUTE_H
