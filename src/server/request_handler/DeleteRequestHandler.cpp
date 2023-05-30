#include "DeleteRequestHandler.h"
#include "../../cgi_executor/CgiExecutor.h"

DeleteRequestHandler::DeleteRequestHandler(const Config *config) : RequestHandler(config)
{}

std::string DeleteRequestHandler::handle_request_str(const HttpRequest &request)
{
	HttpResponse response;
	Route *route = find_route(request.get_path());

	// TODO : mettre en commun avec les autres handlers ===========
	if (route == NULL)
	{
		response.set_status(404, "Not Found");
		return response.to_string();
	}
	if (!is_method_allowed(route, request))
	{
		response.set_status(405, "Method Not Allowed");
		return response.to_string();
	}
	// ============================================================

	// TODO : mettre en commun ?
	if (route->cgi.isSome())
	{
		const CgiConfig &cgi = route->cgi.unwrap();
		std::string cgi_str_response = CgiExecutor::execute(request, *config_, cgi, *route);
		return parseCGIResponse(cgi_str_response).to_string();
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
	return response.to_string();
}
