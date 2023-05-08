#include <iostream>
#include "Route.h"
#include "yaml_helper.h"

Route *Route::parse(std::string &route_config)
{
	Route *route = new Route();

	route_config = unpad_from_left(route_config, 4, true);
	route->methods = parse_methods(route_config);
	{
		Option<std::string> line = find_key_value_line(route_config, "redirection", true);
		if (line.isSome())
			route->redirection = Option<std::string>::Some(get_value_from_line(line.unwrap()));
	}
	{
		Option<std::string> line = find_key_value_line(route_config, "root", true);
		if (line.isSome())
			route->root = Option<std::string>::Some(get_value_from_line(line.unwrap()));
	}
	{
		route->repertory_listing = false;
		Option<std::string> line = find_key_value_line(route_config, "repertory_listing", true);
		if (line.isSome())
		{
			std::string value = get_value_from_line(line.unwrap());
			if (value != "true" && value != "false")
				throw std::runtime_error("Invalid config file, repertory_listing value must be true or false");
			route->repertory_listing = value == "true";
		}
	}
	{
		Option<std::string> line = find_key_value_line(route_config, "index", true);
		if (line.isSome())
			route->index = Option<std::string>::Some(get_value_from_line(line.unwrap()));
	}
	{
		Option<std::string> line = find_key_value_line(route_config, "cgi", true);
		if (line.isSome())
		{
			std::string value = get_value_from_line(line.unwrap());
			route->cgi = Option<CgiConfig>::Some(CgiConfig::parse(value));
		}
	}
	{
		Option<std::string> line = find_key_value_line(route_config, "upload_directory", true);
		if (line.isSome())
			route->upload_directory = Option<std::string>::Some(get_value_from_line(line.unwrap()));
	}
	return route;
}

void Route::log() const
{
	std::cout << "methods: " << methods << ", redirection: " << redirection << ", root: " << root
			  << ", repertory_listing: " << repertory_listing << ", index: " << index << ", cgi: " << cgi
			  << ", upload_directory: " << upload_directory << std::endl;
}

const short Route::DEFAULT::METHODS = Http::Methods::GET;
const bool Route::DEFAULT::REPERTORY_LISTING = false;
const Option<std::string> Route::DEFAULT::INDEX = Option<std::string>::Some("index.html");
const Option<std::string> Route::DEFAULT::ROOT = Option<std::string>::Some("PWD/www/");