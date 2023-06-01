#include "PostRequestHandler.h"
#include "../../cgi_executor/CgiExecutor.h"

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
        return handle_error_static(404, "Not Found (file/dir)", config_);

    HttpResponse response;

    if (is_directory)
    {
        if (route->index.isSome()) {
            requested_path += route->index.unwrap();
        }    
        else {
            return handle_error_static(403, "Forbidden", config_);
        }
    }
    if (is_path_file(requested_path) && route->cgi.isSome())
	{
		const CgiConfig &cgi = route->cgi.unwrap();
		bool good_extension =
				requested_path.rfind(cgi.file_extension) == (requested_path.size() - cgi.file_extension.size());
		if (good_extension)
		{
			std::string cgi_response;
			try
			{
				cgi_response = CgiExecutor::execute(request, *config_, cgi, *route);
			}
            catch (HttpResponse &response)
            {
                return response;
            }
			catch (const std::exception &e)
			{
				std::cout << "CgiExecutor::execute: " << e.what() << std::endl;
				return handle_error_static(500, "Internal Server Error (CGI)", config_);
			}
			return parseCGIResponse(cgi_response);
		}
	}
    return handle_error_static(403, "Forbidden", config_);
}

PostRequestHandler::PostRequestHandler(
        const Config *config) : RequestHandler(config)
{}

