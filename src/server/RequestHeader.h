#ifndef REQUEST_HEADER_H
#define REQUEST_HEADER_H
#include <iostream>

class RequestHeader
{
public:
	struct Method
	{
		enum
		{
			GET = 0,
			POST = 1,
			/* TODO */
		};
	};

	RequestHeader(const std::string &request);

	~RequestHeader();

// private:
	RequestHeader();

	int _method;
	int _httpVersion;
	std::string _path; // uri
};

#endif // !REQUEST_HEADER_H
