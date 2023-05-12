#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "../config/Config.h"

class RequestHandler {
public:
    virtual ~RequestHandler() {}
    RequestHandler(const Config *config);
    virtual HttpResponse handle_request(const HttpRequest& request) = 0;
protected:
    const Config *config_;

    Route* find_route(const std::string& requested_path) const;
    bool is_method_allowed(const Route* route, const HttpRequest& request);
    std::string create_error_html(int error_code, const std::string& error_phrase) const;
    HttpResponse handle_error(int error_code, const std::string& error_phrase);
    Option<HttpResponse> parse(const HttpRequest& request);
};

class GetRequestHandler : public RequestHandler {
public:
    GetRequestHandler(const Config* config);

    virtual HttpResponse handle_request(const HttpRequest& request);
};

class PostRequestHandler : public RequestHandler {
public:
    PostRequestHandler(const Config* config);

    virtual HttpResponse handle_request(const HttpRequest& request);
};

#endif
