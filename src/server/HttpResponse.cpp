#include "HttpResponse.h"
#include <sstream>
#include <ctime>

std::string get_http_date() {
    char buffer[1000];
    time_t now = time(0);
    struct tm *timeinfo = gmtime(&now);

    strftime(buffer, 1000, "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
    return buffer;
}

HttpResponse::HttpResponse() : _status_code(200), _status_description("OK"), _version("HTTP/1.1") {
    add_header("Date", get_http_date());
}

void HttpResponse::set_version(const std::string& version) {
    _version = version;
}

void HttpResponse::set_status(int status_code, const std::string& status_description) {
    _status_code = status_code;
    _status_description = status_description;
}

void HttpResponse::add_header(const std::string& header_name, const std::string& header_value) {
    _headers[header_name] = header_value;
}

void HttpResponse::set_body(const std::string& body) {
    _body = body;
}

std::string HttpResponse::to_string() const {
    std::ostringstream response_stream;

    response_stream << _version << " " << _status_code << " " << _status_description << "\r\n";

    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it) {
        response_stream << it->first << ": " << it->second << "\r\n";
    }

    response_stream << "Content-Length: " << _body.size() << "\r\n\r\n";
    response_stream << _body;

    return response_stream.str();
}