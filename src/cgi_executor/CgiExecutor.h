#ifndef CGIEXECUTOR_H
#define CGIEXECUTOR_H

#include <string>
#include "../server/HttpRequest.h"
#include "../server/HttpResponse.h"

class CgiExecutor
{
public:
	void execute(const HttpRequest& request, HttpResponse& response);
};

#endif //CGIEXECUTOR_H
