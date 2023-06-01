#ifndef CGIEXECUTOR_H
#define CGIEXECUTOR_H

#include <string>
#include "../http/HttpRequest.h"
#include "../http/HttpResponse.h"
#include "../config/Config.h"

class CgiExecutor
{
public:
	static std::string
	execute(const HttpRequest &request, const Config &config, const CgiConfig &cgi_config, const Route &route);
};

#endif //CGIEXECUTOR_H
