#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include "../HttpRequest.h"
#include "../HttpResponse.h"
#include "../../config/Config.h"

std::pair<std::string, Option<std::string> > get_path_and_file(const std::string &path);
std::string dir_listing(std::string const &path, std::string const &uri);
bool is_path_dir(const std::string &path);
bool is_path_file(const std::string& path);
HttpResponse parseCGIResponse(const std::string &cgiOutput);
std::string get_content_type(const std::string &path);
std::string real_path(const Route &route, const HttpRequest& request);
bool check_path(const std::string& path);

class RequestHandler {
public:
    virtual                 ~RequestHandler() {}
    virtual                 HttpResponse handle_request(const HttpRequest& request) = 0;
    RequestHandler(const Config *config);
    HttpResponse            handle_error(int error_code, const std::string& error_phrase);

protected:
    const Config*           config_;

    Route*                  find_route(const std::string& requested_path) const;
    bool                    is_method_allowed(const Route* route, const HttpRequest& request);
    std::string             create_error_html(int error_code, const std::string& error_phrase) const;
    Option<HttpResponse>    checkRequestValidity(const HttpRequest& request);
    HttpResponse            handle_redirection(const Route* route);

    Route * getRouteOrThrowResponse(const HttpRequest &request);

};

#endif
