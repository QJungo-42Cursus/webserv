#include "Header.h"
#include <string>
#include <sstream>

Header::Header(int responseCode, std::string content, int contentType) : _responseCode(responseCode), _content(content), _contentType(contentType) {}
Header::~Header() {}

std::string Header::toString()
{
	std::string header;

	header += "HTTP/1.1 ";
	header += getStringResponseCode();
	header += "\r\nContent-Type: ";
	header += getStringContentType();
	header += "\r\nContent-Length: ";
	std::stringstream out;
	out << _content.length();
	header += out.str();
	header += "\r\n\r\n";
	header += _content;
	return header;
}

std::string Header::getStringResponseCode()
{
	switch (_responseCode)
	{
	case ResponseCode::OK:
		return "200 OK";
	case ResponseCode::BAD_REQUEST:
		return "400 Bad Request";
	case ResponseCode::NOT_FOUND:
		return "404 Not Found";
	case ResponseCode::INTERNAL_SERVER_ERROR:
		return "500 Internal Server Error";
	default:
		throw std::exception();
	}
}

std::string Header::getStringContentType()
{
	switch (_contentType)
	{
	case ContentType::TEXT_HTML:
		return "text/html";
	case ContentType::TEXT_PLAIN:
		return "text/plain";
	default:
		throw std::exception();
	}
}
