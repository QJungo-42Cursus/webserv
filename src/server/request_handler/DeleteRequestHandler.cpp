#include "DeleteRequestHandler.h"

DeleteRequestHandler::DeleteRequestHandler(
        const Config *config) : RequestHandler(config) {}

HttpResponse DeleteRequestHandler::handle_request(const HttpRequest &request) {
    HttpResponse response;
//    response.set_version(request.get_version());

    Route *route = find_route(request.get_path());
    if (route == NULL) {
        response.set_status(404, "Not Found");
        return response;
    }

    if (!is_method_allowed(route, request)) {
        response.set_status(405, "Method Not Allowed");
        return response;
    }

    std::string file_path;
    if (route->root.isSome()) {
        file_path = route->root.unwrap() + request.get_path();
    } else {
        file_path = request.get_path();
    }

    if (std::remove(file_path.c_str()) != 0) {
        response.set_status(500, "Internal Server Error");
    } else {
        response.set_status(200, "OK");
    }

    return response;
}
