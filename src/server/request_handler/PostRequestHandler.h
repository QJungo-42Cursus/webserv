//
// Created by qjungo on 5/30/23.
//

#ifndef WEBSERV_POSTREQUESTHANDLER_H
#define WEBSERV_POSTREQUESTHANDLER_H

#include "RequestHandler.h"

class PostRequestHandler : public RequestHandler {
public:
    PostRequestHandler(const Config* config);

    virtual HttpResponse handle_request(const HttpRequest& request);
};

#endif //WEBSERV_POSTREQUESTHANDLER_H
