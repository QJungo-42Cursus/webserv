#include "GetRequestHandler.h"
#include "../../cgi_executor/CgiExecutor.h"



HttpResponse GetRequestHandler::handle_request(const HttpRequest &request)
{
    // TODO: en commun ========================================================
	Route *route;
    try
    {
        route = getRouteOrThrowResponse(request);
    }
    catch (HttpResponse &response)
    {
        return response;
    }
    catch (std::exception &e)
    {
        return handle_error(500, "Internal Server Error : " + std::string(e.what()));
    }

	std::string requested_path = real_path(*route, request);
	if (requested_path.empty())
	{
		return handle_error(404, "Not Found (path)");
	}
	if (*(requested_path.end() - 1) == '/')
	{
		// on doit l'enlever pour les fichiers (sinon le is_path_file ne marche pas)
		requested_path = requested_path.substr(0, requested_path.size() - 1);
	}

	std::cout << std::endl << "Requested path: " << requested_path << std::endl;

	bool is_file = is_path_file(requested_path);
	bool is_directory = is_path_dir(requested_path);

	if (!is_file && !is_directory)
		return handle_error(404, "Not Found (file/dir)");

    // ========================================================================

	HttpResponse response;
	if (is_directory)
	{
		requested_path += "/";
		if (route->repertory_listing)
		{
			response.set_body(dir_listing(requested_path, request.get_path()));
			response.set_status(200, "OK");
			response.add_header("Content-Type", "text/html");
			return response;
		}
		else if (route->index.isSome() /* && is_path_root_of_route */ )
		{
			// TODO dans le cas ou la route est definie en arriere
			//	example : route '/', recherche '/photo/' -> la route est donc '/'
			//	si un index est defini, ne devrait t'il pas marche qu'avec la racine de la route ?
			requested_path += route->index.unwrap();
			std::cout << "Index path: " << requested_path << std::endl;

			// TODO ne rentre pas dans le cgi ?
		}
		else
		{
			return handle_error(403, "Forbidden");
		}
	}
	else if (route->cgi.isSome())
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
			catch (const std::exception &e)
			{
				std::cout << "CgiExecutor::execute: " << e.what() << std::endl;
				return handle_error(500, "Internal Server Error (CGI)");
			}
			// HttpResponse cgi_res = parseCGIResponse(cgi_response);
			response.set_body(cgi_response);
			response.set_status(200, "OK");
			response.add_header("Content-Type", "text/html");
			return response;
		}
	}

	std::ifstream file(requested_path.c_str(), std::ios::in | std::ios::binary);

	if (!file)
	{
		return handle_error(404, "Not Found");
	}
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	response.set_body(content);
	response.set_status(200, "OK");
	response.add_header("Content-Type", get_content_type(requested_path));
	return response;
}

GetRequestHandler::GetRequestHandler(const Config *config) : RequestHandler(config)
{}

