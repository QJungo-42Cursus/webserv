#ifndef HEADER_H
#define HEADER_H

#include <string>

class Header
{
public:
	enum ResponseCode
	{
		OK = 200,
		BAD_REQUEST = 400,
		NOT_FOUND = 404,
		INTERNAL_SERVER_ERROR = 500
	};

	enum ContentType
	{
		TEXT_HTML = 0,
		TEXT_PLAIN = 2,
		/* TODO */
	};

	/* Canonic form */
	Header();
	Header(int responseCode, std::string content, ContentType contentType);
	Header(const Header &other);
	Header &operator=(const Header &other);
	~Header();

	/* Methods */
	std::string getHeader();
	std::string getStringResponseCode();
	std::string getStringContentType();

private:
	const int _responseCode;
	const std::string _content;
	const ContentType _contentType;
};

#endif // HEADER_H