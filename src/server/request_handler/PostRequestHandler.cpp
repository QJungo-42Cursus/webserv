#include "PostRequestHandler.h"

HttpResponse PostRequestHandler::handle_request(const HttpRequest &request)
{
    Route *route;
    try
    {
        route = getRouteOrThrowResponse(request);
    }
    catch (HttpResponse &response)
    {
        return response;
    }
    std::string requested_path = real_path(*route, request);
    bool is_file = is_path_file(requested_path);
    bool is_directory = is_path_dir(requested_path);
    if (!is_file && !is_directory)
        return handle_error(404, "Not Found (file/dir)");

    HttpResponse response;







    // TODO: POST HANDLER
    response.set_status(200, "OK");
    return response;
}

PostRequestHandler::PostRequestHandler(
        const Config *config) : RequestHandler(config)
{}

