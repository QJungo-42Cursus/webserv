#ifndef ResponseHeader_H
#define ResponseHeader_H

#include <string>

class ResponseHeader
{
public:
	struct ResponseCode
	{
		enum
		{
			OK = 200,
			BAD_REQUEST = 400,
			NOT_FOUND = 404,
			INTERNAL_SERVER_ERROR = 500
		};
	};

	struct ContentType
	{
		enum
		{
			TEXT_HTML = 0,
			TEXT_PLAIN = 2,
			/* TODO */
		};
	};


	/* Canonic form */
	ResponseHeader(int responseCode, std::string content, int contentType);
	ResponseHeader(const ResponseHeader &other);
	ResponseHeader &operator=(const ResponseHeader &other);
	~ResponseHeader();

	/* Methods */
	std::string toString();
	std::string getStringResponseCode();
	std::string getStringContentType();
	static ResponseHeader parse(const std::string &str);

private:
	ResponseHeader();

	const int _responseCode;
	const std::string _content;
	const int _contentType;
};

#endif // ResponseHeader_H