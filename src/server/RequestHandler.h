#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include "HttpRequest.h"
#include "HttpResponse.h"

class RequestHandler {
public:
    virtual ~RequestHandler() {}

    virtual HttpResponse handle_request(const HttpRequest& request) = 0;
};

class GetRequestHandler : public RequestHandler {
public:
    virtual HttpResponse handle_request(const HttpRequest& request);
};

class PostRequestHandler : public RequestHandler {
public:
    virtual HttpResponse handle_request(const HttpRequest& request);
};

class DeleteRequestHandler : public RequestHandler {
public:
    virtual HttpResponse handle_request(const HttpRequest& request);
};

#endif
