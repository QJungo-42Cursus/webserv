#include "RequestHeader.h"
#include "../http/http.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

static Http::Methods::EMethods getMethod(const std::string &firstLine)
{
	std::string method = firstLine.substr(0, firstLine.find(" "));
	if (method == "GET")
		return Http::Methods::GET;
	else if (method == "POST")
		return Http::Methods::POST;
	else if (method == "DELETE")
		return Http::Methods::DELETE;
	else if (method == "PUT" || method == "HEAD" || method == "OPTIONS" || method == "TRACE" || method == "CONNECT")
		throw Http::UnimplementedMethodException();
	else
		throw Http::UnknownMethodException();
}

RequestHeader::RequestHeader(const std::string &request)
{
	std::string firstLine;
	{
		/// First line
		if (request.empty())
			throw Http::InvalidRequestException();
		firstLine = request.substr(0, request.find("\r\n"));
		if (firstLine.empty())
			throw Http::InvalidRequestException();

		/// Method
		_method = getMethod(firstLine);

		/// Path
		int pathStart = firstLine.find("/");
		int pathEnd = firstLine.find(" ", pathStart);
		if (pathStart == 0 || pathEnd == pathStart)
			throw Http::InvalidRequestException();
		_path = firstLine.substr(pathStart, pathEnd - pathStart);

		/// HTTP version
		int httpVersionStart = firstLine.find("HTTP/");
		int httpVersionEnd = firstLine.find("\r\n", httpVersionStart);
		std::string httpVersion = firstLine.substr(httpVersionStart, httpVersionEnd - httpVersionStart);
		if (httpVersion != "HTTP/1.1")
		{
			throw Http::UnsupportedVersionException();
		}
	}

	std::string request_n = request;
	request_n.erase(0, request_n.find("\r\n") + 2);

	size_t pos = 0;
	std::string token;
	std::string delimiter = "\r\n";
	while ((pos = request_n.find(delimiter)) != std::string::npos)
	{
		size_t n_pos = request_n.find("\r\n\r\n");
		if (n_pos != std::string::npos && n_pos <= pos)
			break;
		token = request_n.substr(0, pos);
		size_t middle = token.find(": ");
		if (middle == std::string::npos)
		{
			std::cout << request << "Invalid header: '" << token << "'" << std::endl;
			throw Http::InvalidRequestException();
		}
		std::string key = token.substr(0, middle);
		std::string value = token.substr(middle, token.length());
		_headers[key] = value;
		request_n.erase(0, pos + delimiter.length());
	}
	// TODO: last header
	// TODO: body
}

const std::string &RequestHeader::getPath() const
{
	return _path;
}

void RequestHeader::logHeaders() const
{
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
	{
		std::cout << "[" << it->first << "] '" << it->second << "'" << std::endl;
	}
}
