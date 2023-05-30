#ifndef CGIEXECUTOR_H
#define CGIEXECUTOR_H

#include <string>
#include "../server/HttpRequest.h"
#include "../server/HttpResponse.h"
#include "../config/Config.h"

class CgiExecutor {
public:
    static std::string execute(const HttpRequest &request, const Config &config, const CgiConfig &cgi_config, const Route &route);
};

#endif //CGIEXECUTOR_H
