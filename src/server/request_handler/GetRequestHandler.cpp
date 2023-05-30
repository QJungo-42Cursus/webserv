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

    requested_path = real_path(*route, request);

    bool is_file = is_path_file(requested_path);
    bool is_directory = is_path_dir(requested_path);

    if (!is_file && !is_directory)
        return handle_error(404, "Not Found");

    if (!is_file && is_directory)
    {
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

    else if (is_file && route->cgi.isSome()) {
        const CgiConfig &cgi = route->cgi.unwrap();
        bool good_extension = requested_path.rfind(cgi.file_extension) == (requested_path.size() - cgi.file_extension.size());
        if (good_extension) {
            std::string cgi_response = CgiExecutor::execute(request, *config_, cgi, *route);
            // HttpResponse cgi_res = parseCGIResponse(cgi_response);
            response.set_body(cgi_response);
            response.set_status(200, "OK");
            response.add_header("Content-Type", "text/html");
            return response;
        }
    }
    
    
    std::ifstream file(requested_path.c_str(), std::ios::in | std::ios::binary);


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

