#include "RequestHeader.h"

RequestHeader::RequestHeader(const std::string &request)
{
	std::string firstLine = request.substr(0, request.find("\r\n"));

	if (firstLine.substr(0, firstLine.find(" ")) == "GET")
		_method = Method::GET;
	else if (firstLine.substr(0, firstLine.find(" ")) == "POST")
		_method = Method::POST;
	else
		throw std::exception();

	int pathStart = firstLine.find("/");
	int pathEnd = firstLine.find(" ", pathStart);
	_path = firstLine.substr(pathStart, pathEnd - pathStart);

	int httpVersionStart = firstLine.find("HTTP/");
	int httpVersionEnd = firstLine.find("\r\n", httpVersionStart);
	std::string httpVersion = firstLine.substr(httpVersionStart, httpVersionEnd - httpVersionStart);
	if (httpVersion == "HTTP/1.1")
		_httpVersion = 1;
	else if (httpVersion == "HTTP/1.0")
		_httpVersion = 0;
	else
		throw std::exception();
}

RequestHeader::~RequestHeader()
{
}

RequestHeader::RequestHeader()
{
}
