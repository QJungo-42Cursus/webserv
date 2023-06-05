#ifndef GETREQUESTHANDLER_H
#define GETREQUESTHANDLER_H
#include "RequestHandler.h"

class GetRequestHandler : public RequestHandler {
public:
    GetRequestHandler(const Config* config);

    virtual HttpResponse handle_request(const HttpRequest& request);
};

#endif //GETREQUESTHANDLER_H
