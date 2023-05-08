#ifndef CGIEXECUTOR_H
#define CGIEXECUTOR_H

#include <string>
#include "../server/HttpRequest.h"
#include "../server/HttpResponse.h"
#include "../config/Config.h"

class CgiExecutor
{
public:
	std::string execute(const HttpRequest& request, const Config& config);
};

#endif //CGIEXECUTOR_H
