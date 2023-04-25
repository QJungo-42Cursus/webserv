#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include <exception>
namespace Http
{
	struct UnknownMethodException : public std::exception
	{
		const char *what() const throw()
		{
			return "Unknown http method";
		}
	};

	struct UnimplementedMethodException : public std::exception
	{
		const char *what() const throw()
		{
			return "Http method not implemented";
		}
	};

	struct InvalidRequestException : public std::exception
	{
		const char *what() const throw()
		{
			return "Invalid http request";
		}
	};

	struct EmptyRequestException : public InvalidRequestException
	{
		const char *what() const throw()
		{
			return "Http request is empty";
		}
	};

	

	struct UnsupportedVersionException : public std::exception
	{
		const char *what() const throw()
		{
			return "Unsupported http version";
		}
	};
}

#endif // EXCEPTIONS_H