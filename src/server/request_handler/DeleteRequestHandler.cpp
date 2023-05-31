#include "DeleteRequestHandler.h"
#include "../../cgi_executor/CgiExecutor.h"

DeleteRequestHandler::DeleteRequestHandler(const Config *config) : RequestHandler(config)
{}

HttpResponse DeleteRequestHandler::handle_request(const HttpRequest &request)
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




	// TODO : mettre en commun ?
	if (route->cgi.isSome())
	{
		const CgiConfig &cgi = route->cgi.unwrap();
		std::string cgi_str_response = CgiExecutor::execute(request, *config_, cgi, *route);
		return parseCGIResponse(cgi_str_response);
	}
	std::string file_path = real_path(*route, request);
	if (std::remove(file_path.c_str()) != 0)
	{
		response.set_status(500, "Internal Server Error");
	}
	else
	{
		response.set_status(200, "OK");
	}
	return response;
}
