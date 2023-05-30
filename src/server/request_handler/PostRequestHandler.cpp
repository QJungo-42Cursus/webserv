//
// Created by qjungo on 5/30/23.
//

#include "PostRequestHandler.h"

HttpResponse PostRequestHandler::handle_request(const HttpRequest &request)
{
    HttpResponse response;

    Route *route = find_route(request.get_path());
    if (route == NULL) {
        return handle_error(404, "Not Found");
    }

    if (!is_method_allowed(route, request)) {
        return handle_error(405, "Method Not Allowed");
    }

    // TODO: POST HANDLER
    response.set_status(200, "OK");
    return response;
}

PostRequestHandler::PostRequestHandler(
        const Config *config) : RequestHandler(config)
{}

