#ifndef POSTREQUESTHANDLER_H
#define POSTREQUESTHANDLER_H

#include "RequestHandler.h"

class PostRequestHandler : public RequestHandler
{
public:
    PostRequestHandler(const Config *config);

    virtual HttpResponse handle_request(const HttpRequest &request);
};

#endif //POSTREQUESTHANDLER_H
