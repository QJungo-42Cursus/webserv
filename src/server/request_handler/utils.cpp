#include "RequestHandler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <sys/stat.h>
#include <dirent.h>
#include "../../cgi_executor/CgiExecutor.h"

bool is_path_dir(const std::string &path) {
    struct stat s;
    if (stat(path.c_str(), &s) == 0) {
        return S_ISDIR(s.st_mode);
    }
    return false;
}

bool is_path_file(const std::string& path) {
    struct stat s;
    if(stat(path.c_str(), &s) == 0) {
        return S_ISREG(s.st_mode);
    }
    return false;
}

std::string real_path(const Route &route, const HttpRequest& request) {
    std::string requested_path;
    std::string route_path;
    std::string route_root;

    requested_path = request.get_path();
    requested_path.erase(requested_path.find_last_not_of("%20")+1);
    route_path = route.name;
    route_root = route.root;

    if (route_root[route_root.size() - 1] != '/')
        route_root += '/';

    if (requested_path[requested_path.size() - 1] != '/')
        requested_path += '/';

    size_t pos = requested_path.find(route_path);

    if (pos != std::string::npos) {
        requested_path.replace(pos, route_path.length(), route_root);
    }

    if (requested_path[requested_path.size() - 1] == '/')
        requested_path.erase(requested_path.size() - 1);

    bool is_directory = is_path_dir(requested_path);
    if (is_directory && requested_path[requested_path.size() - 1] != '/')
            requested_path += '/';
    return requested_path;
}

/// This will return a string containing the HTML code for a directory listing
std::string dir_listing(std::string const &path, std::string const &uri)
{
    std::cout << std::endl << "===================" << std::endl << std::endl;
    std::cout << "dir_listing: " << path << std::endl << "uri: " << uri << std::endl;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) == NULL)
        throw std::runtime_error("Could not open directory");

    std::vector<std::string> files;
    while ((ent = readdir(dir)) != NULL)
        files.push_back(ent->d_name);
    std::sort(files.begin(), files.end());

    closedir(dir);

    std::string html = "<html><head><title>Index of ";
    html += path + "</title></head><body><h1>Index of " + uri + "</h1><hr>\n";
    for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it)
    {
        std::string href;
        if (uri.rfind('/') == uri.length() - 1)
            href = uri + *it;
        else
            href = uri + "/" + *it;
        html += "<a href=\"" + href + "\">" + *it + "/</a><br>\n";
    }
    html += "<hr></body></html>";
    return html;
}

std::pair<std::string, Option<std::string> > get_path_and_file(const std::string &path)
{
    std::string::size_type idx = path.rfind('/');
    if (idx != std::string::npos)
    {
        std::string file = path.substr(idx + 1);
        std::string n_path = path.substr(0, idx + 1);
        Option<std::string> opt_file = Option<std::string>::Some(file);
        if (file.empty())
            opt_file = Option<std::string>::None();
        return std::make_pair(n_path, opt_file);
    }
    return std::make_pair(path, Option<std::string>::None());
}


std::string get_content_type(const std::string &path) {
    std::string::size_type idx = path.rfind('.');
    if (idx != std::string::npos) {
        std::string extension = path.substr(idx + 1);
        if (extension == "html" || extension == "htm") {
            return "text/html";
        }
    }
    return "text/plain";
}

HttpResponse parseCGIResponse(const std::string &cgiOutput) {
    HttpResponse response;

    response.set_status(200, "OK");
    // Split headers and body
    std::string::size_type separatorPos = cgiOutput.find("\r\n\r\n");
    std::string headersStr = cgiOutput.substr(0, separatorPos);
    std::string body = cgiOutput.substr(separatorPos + 4);

    // Split headers
    std::istringstream headersStream(headersStr);
    std::string headerLine;
    while (std::getline(headersStream, headerLine)) {
        std::string::size_type colonPos = headerLine.find(": ");
        if (colonPos != std::string::npos) {
            std::string headerName = headerLine.substr(0, colonPos);
            std::string headerValue = headerLine.substr(colonPos + 2);

            if (headerName == "Status") {
                // Parse status code and reason phrase
                std::istringstream statusStream(headerValue);
                int statusCode;
                std::string reasonPhrase;
                statusStream >> statusCode;
                std::getline(statusStream, reasonPhrase);
                response.set_status(statusCode, reasonPhrase);
            } else {
                response.add_header(headerName, headerValue);
            }
        }
    }

    response.set_body(body);

    return response;
}
