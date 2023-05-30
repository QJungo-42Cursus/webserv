//
// Created by qjungo on 5/30/23.
//

#ifndef WEBSERV_GETREQUESTHANDLER_H
#define WEBSERV_GETREQUESTHANDLER_H
#include "RequestHandler.h"

class GetRequestHandler : public RequestHandler {
public:
    GetRequestHandler(const Config* config);

    virtual HttpResponse handle_request(const HttpRequest& request);
};

#endif //WEBSERV_GETREQUESTHANDLER_H
