#ifndef METHODS_H
#define METHODS_H
namespace Http
{
	namespace Methods
	{
		enum EMethods
		{
			GET = 0b0001,
			POST = 0b0010,
			DELETE = 0b0100,
			PUT,
			HEAD,
			OPTIONS,
			TRACE,
			CONNECT,
			UNKNOWN
		};
	}
}
#endif