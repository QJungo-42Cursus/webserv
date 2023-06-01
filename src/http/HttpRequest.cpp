
#include "HttpRequest.h"
#include <iostream>
#include <sstream>
#include "Methods.h"
#include "HttpResponse.h"

HttpRequest::HttpRequest(const std::string& raw_request) : raw_(raw_request) {
    parse_request(raw_request);
}

int HttpRequest::get_method() const {
    return method_;
}

std::string HttpRequest::get_path() const {
    return path_;
}

const std::string HttpRequest::get_raw() const {
    return raw_;
}

std::string HttpRequest::get_version() const {
    return version_;
}

std::map<std::string, std::string> HttpRequest::get_headers() const {
    return headers_;
}

std::string HttpRequest::get_body() const {
    return body_;
}

void HttpRequest::parse_request(const std::string& raw_request) {
    std::istringstream request_stream(raw_request);
    std::string line;

    // Parse the request line
    std::getline(request_stream, line);
    parse_request_line(line);

    // Parse headers
    while (std::getline(request_stream, line) && line != "\r") {
        parse_header(line);
    }

    // Parse body (if any)
    if (method_ == Http::Methods::POST || method_ == Http::Methods::PUT) {
        std::getline(request_stream, body_);
    }
}

void HttpRequest::parse_request_line(const std::string& line) {
    std::istringstream line_stream(line);
    std::string method_str;
    line_stream >> method_str >> path_ >> version_;

    if (method_str == "GET") {
        method_ = Http::Methods::GET;
    } else if (method_str == "POST") {
        method_ = Http::Methods::POST;
    } else if (method_str == "DELETE") {
        method_ = Http::Methods::DELETE;
    } else if (method_str == "PUT") {
        method_ = Http::Methods::PUT;
    } else {
        method_ = Http::Methods::UNKNOWN;
    }
}

void HttpRequest::parse_header(const std::string& line) {
    size_t colon_pos = line.find(':');
    if (colon_pos != std::string::npos) {
        std::string key = line.substr(0, colon_pos);
        std::string value = line.substr(colon_pos + 2); // Skip ": "
        headers_[key] = value;
    }
}

void HttpRequest::log() const
{
	std::cout << "method: " << method_ << std::endl;
	std::cout << "path: " << path_ << std::endl;
	std::cout << "version: " << version_ << std::endl;
	std::cout << "headers: " << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = headers_.begin(); it != headers_.end(); ++it)
	{
		std::cout << it->first << ": " << it->second << std::endl;
	}
	std::cout << "body: " << body_ << std::endl;
}
