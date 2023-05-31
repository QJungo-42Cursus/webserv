//
// Created by qjungo on 5/30/23.
//

#include "PutRequestHandler.h"

HttpResponse PutRequestHandler::handle_request(const HttpRequest &request)
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

    HttpResponse response;

    if (is_directory)
    {
        if (route->index.isSome()) {
            requested_path += route->index.unwrap();
        }    
        else {
            return handle_error(403, "Forbidden");
        }
    }

	std::string parent_dir = requested_path.substr(0, requested_path.find_last_of('/'));
    if (!is_path_dir(parent_dir)) {
        return handle_error(409, "Conflict");
    }
    std::ofstream file(requested_path.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
	if (!file.is_open())
		return handle_error(500, "Internal Server Error");
	file.write(request.get_body().c_str(), request.get_body().size());
	file.close();

    response.set_status(file.tellp() == 0 ? 201 : 200, file.tellp() == 0 ? "Created" : "OK");

    return response;
	
}

PutRequestHandler::PutRequestHandler(
        const Config *config) : RequestHandler(config)
{}

