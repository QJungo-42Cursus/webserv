#ifndef DELETEREQUESTHANDLER_H
#define DELETEREQUESTHANDLER_H

#include "../RequestHandler.h"

class DeleteRequestHandler : public RequestHandler {
public:
    explicit DeleteRequestHandler(const Config *config);

    virtual HttpResponse handle_request(const HttpRequest &request);
};

#endif // DELETEREQUESTHANDLER_H
