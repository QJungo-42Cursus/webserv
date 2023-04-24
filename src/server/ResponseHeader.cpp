#include "ResponseHeader.h"
#include <string>
#include <sstream>

ResponseHeader::ResponseHeader(
	int responseCode,
	std::string content,
	int contentType)
	: _responseCode(responseCode),
	  _content(content),
	  _contentType(contentType)
{
}

ResponseHeader::~ResponseHeader() {}

std::string ResponseHeader::toString()
{
	std::string ResponseHeader;

	ResponseHeader += "HTTP/1.1 ";
	ResponseHeader += getStringResponseCode();
	ResponseHeader += "\r\nContent-Type: ";
	ResponseHeader += getStringContentType();
	ResponseHeader += "\r\nContent-Length: ";
	std::stringstream out;
	out << _content.length();
	ResponseHeader += out.str();
	ResponseHeader += "\r\n\r\n";
	ResponseHeader += _content;
	return ResponseHeader;
}

std::string ResponseHeader::getStringResponseCode()
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

std::string ResponseHeader::getStringContentType()
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