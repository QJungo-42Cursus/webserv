#ifndef PUTREQUESTHANDLER_H
#define PUTREQUESTHANDLER_H

#include "RequestHandler.h"

class PutRequestHandler : RequestHandler
{
public:
    PutRequestHandler(const Config *config);

    virtual HttpResponse handle_request(const HttpRequest &request);
};

#endif //PUTREQUESTHANDLER_H
