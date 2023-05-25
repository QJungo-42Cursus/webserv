#include "DeleteRequestHandler.h"
#include "../../cgi_executor/CgiExecutor.h"

DeleteRequestHandler::DeleteRequestHandler(const Config *config) : RequestHandler(config) {}

std::string DeleteRequestHandler::handle_request_str(const HttpRequest &request) {
    HttpResponse response;
    Route *route = find_route(request.get_path());
    if (route == NULL) {
        response.set_status(404, "Not Found");
        return response.to_string();
    }

    if (!is_method_allowed(route, request)) {
        response.set_status(405, "Method Not Allowed");
        return response.to_string();
    }

    if (route->cgi.isSome()) {
        const CgiConfig &cgi = route->cgi.unwrap();
        std::string cgi_response = CgiExecutor::execute(request, *config_, cgi);
        // TODO return the string and not the response ??
    } else {
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
    }
    return response.to_string();
}
