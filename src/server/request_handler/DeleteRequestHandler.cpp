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
    catch (const HttpResponse &response)
    {
        return response;
    }
    catch (const std::exception &e)
    {
        return handle_error(500, "Internal Server Error: " + std::string(e.what()));
    }


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
