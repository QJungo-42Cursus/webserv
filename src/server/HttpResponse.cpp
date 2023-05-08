#include "HttpResponse.h"
#include <sstream>

HttpResponse::HttpResponse(int status_code, const std::string& reason_phrase,
                           const std::map<std::string, std::string>& headers, const std::string& body)
    : status_code_(status_code), reason_phrase_(reason_phrase), headers_(headers) {
    set_body(body);
}

std::string HttpResponse::to_string() const {
    std::ostringstream response_stream;

    // Add status line
    response_stream << "HTTP/1.1 " << status_code_ << " " << reason_phrase_ << "\r\n";

    // Add headers
    for (std::map<std::string, std::string>::const_iterator it = headers_.begin(); it != headers_.end(); ++it) {
        response_stream << it->first << ": " << it->second << "\r\n";
    }

    // Add a blank line to separate headers and body
    response_stream << "\r\n";

    // Add body
    response_stream << body_;

    return response_stream.str();
}

void HttpResponse::set_body(const std::string& body) {
    body_ = body;
    // TODO version C98
    // headers_["Content-Length"] = std::to_string(body_.size());
}
