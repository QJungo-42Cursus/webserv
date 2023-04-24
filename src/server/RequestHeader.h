#ifndef REQUEST_HEADER_H
#define REQUEST_HEADER_H
#include <iostream>
#include <map>

class RequestHeader
{
public:
	RequestHeader(const std::string &request);
	~RequestHeader() {}
	const std::string &getPath() const;
	void logHeaders() const;

private:
	RequestHeader() {}
	int _method;
	std::string _path;
	std::map<std::string, std::string> _headers;
};

#endif // !REQUEST_HEADER_H
