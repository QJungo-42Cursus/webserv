//
// Created by qjungo on 5/30/23.
//

#include "GetRequestHandler.h"
#include "../../cgi_executor/CgiExecutor.h"

HttpResponse GetRequestHandler::handle_request(const HttpRequest &request)
{
    Option<HttpResponse> res = parse(request);
    if (res.isSome()) {
        return (res.unwrap());
    }
    HttpResponse response;

    std::pair<std::string, Option<std::string> > path_and_file = get_path_and_file(
            request.get_path()
    );

    Route *route = find_route(request.get_path());

    if (route == NULL) {
        return handle_error(404, "Not Found");
    }

    if (!is_method_allowed(route, request)) {
        return handle_error(405, "Method Not Allowed");
    }

    if (route->redirection.isSome()) {
        return handle_redirection(route);
    }


    std::string requested_path;
    std::string route_path;
    std::string route_root;

    requested_path = request.get_path();
    requested_path.erase(requested_path.find_last_not_of("%20")+1);
    route_path = route->name;
    route_root = route->root;

    if (route_root[route_root.size() - 1] != '/')
        route_root += '/';

    if (requested_path[requested_path.size() - 1] != '/')
        requested_path += '/';

    size_t pos = requested_path.find(route_path);

    if (pos != std::string::npos) {
        requested_path.replace(pos, route_path.length(), route_root);
    }

    if (requested_path[requested_path.size() - 1] == '/')
        requested_path.erase(requested_path.size() - 1);

    bool is_file = is_path_file(requested_path);
    bool is_directory = is_path_dir(requested_path);

    if (!is_file && !is_directory)
        return handle_error(404, "Not Found");


    if (!is_file && is_directory)
    {
        if (requested_path[requested_path.size() - 1] != '/')
            requested_path += '/';
        if (route->repertory_listing) {
            response.set_body(dir_listing(requested_path, request.get_path()));
            response.set_status(200, "OK");
            response.add_header("Content-Type", "text/html");
            return response;
        }
        else if (route->index.isSome()) {
            requested_path += route->index.unwrap();
            std::cout << "Index path: " << requested_path << std::endl;
        }
        else {
            return handle_error(403, "Forbidden");
        }
    }

    std::ifstream file(requested_path.c_str(), std::ios::in | std::ios::binary);

    if (route->cgi.isSome()) {
        const CgiConfig &cgi = route->cgi.unwrap();
        bool good_extension = requested_path.rfind(cgi.file_extension) == (requested_path.size() - cgi.file_extension.size());
        if (good_extension) {
            std::string cgi_response = CgiExecutor::execute(request, *config_, cgi);
            HttpResponse cgi_res = parseCGIResponse(cgi_response);
            return cgi_res;
        }
    }

    if (!file) {
        return handle_error(404, "Not Found");
    }
    else {
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        response.set_body(content);
        response.set_status(200, "OK");
        response.add_header("Content-Type", get_content_type(requested_path));
    }
    return response;
}

GetRequestHandler::GetRequestHandler(
        const Config *config) : RequestHandler(config)
{}

