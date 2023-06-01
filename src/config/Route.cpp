#include <iostream>
#include "Route.h"
#include "yaml_helper.h"

Route::Route(std::string &route_config, const std::string &route_name)
{
    name = route_name;
    route_config = unpad_from_left(route_config, 4, true);
    methods = parse_methods(route_config);
    {
        Option<std::string> line = find_key_value_line(route_config, "client_max_body_size", true);
        if (line.isSome())
            throw std::runtime_error("Invalid config file, client_max_body_size not allowed in route level");
    }
    {
        Option<std::string> line = find_key_value_line(route_config, "redirection", true);
        if (line.isSome())
            redirection = Option<std::string>::Some(get_value_from_line(line.unwrap()));
    }
    {
        Option<std::string> line = find_key_value_line(route_config, "root", true);
        if (line.isSome())
            root = get_value_from_line(line.unwrap());
        else
            throw std::runtime_error("Invalid config file, root not found in route level");
    }
    {
        repertory_listing = false;
        Option<std::string> line = find_key_value_line(route_config, "repertory_listing", true);
        if (line.isSome())
        {
            std::string value = get_value_from_line(line.unwrap());
            if (value != "true" && value != "false")
                throw std::runtime_error("Invalid config file, repertory_listing value must be true or false");
            repertory_listing = value == "true";
        }
    }
    {
        Option<std::string> line = find_key_value_line(route_config, "index", true);
        if (line.isSome())
            index = Option<std::string>::Some(get_value_from_line(line.unwrap()));
    }
    {
        Option<std::string> line = find_key_value_line(route_config, "cgi", true);
        if (line.isSome())
        {
            std::string value = get_value_from_line(line.unwrap());
            cgi = Option<CgiConfig>::Some(CgiConfig::parse(value));
        }
    }
    {
        Option<std::string> line = find_key_value_line(route_config, "upload_directory", true);
        if (line.isSome())
            upload_directory = Option<std::string>::Some(get_value_from_line(line.unwrap()));
    }
}

void Route::log() const
{
    std::cout << "methods: " << methods << ", redirection: " << redirection << ", root: " << root
              << ", repertory_listing: " << repertory_listing << ", index: " << index << ", cgi: ";
    if (cgi.isSome())
        cgi.unwrap().log();
    else
        std::cout << "None";
    std::cout << ", upload_directory: " << upload_directory << std::endl;
}

Route::~Route()
{}

Route::Route(const Route &other)
{
    methods = other.methods;
    redirection = other.redirection;
    root = other.root;
    repertory_listing = other.repertory_listing;
    index = other.index;
    cgi = other.cgi;
    upload_directory = other.upload_directory;
    name = other.name;
}

Route & Route::operator=(const Route &other)
{
    if (this != &other)
    {
        methods = other.methods;
        redirection = other.redirection;
        root = other.root;
        repertory_listing = other.repertory_listing;
        index = other.index;
        cgi = other.cgi;
        upload_directory = other.upload_directory;
        name = other.name;
    }
    return *this;
}

