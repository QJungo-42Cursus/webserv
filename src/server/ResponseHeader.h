#ifndef ResponseHeader_H
#define ResponseHeader_H
#include "../http/http.h"

#include <string>

class ResponseHeader
{
public:
	/* Canonic form */
	ResponseHeader(Http::ResponseCode::EResponseCode responseCode,
				   std::string content,
				   Http::ContentType::EContentType contentType);
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

	const Http::ResponseCode::EResponseCode _responseCode;
	const std::string _content;
	const Http::ContentType::EContentType _contentType;
};

#endif // ResponseHeader_H