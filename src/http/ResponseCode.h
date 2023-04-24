#ifndef RESPONSECODE_H
#define RESPONSECODE_H

namespace Http
{
	namespace ResponseCode
	{
		enum EResponseCode
		{
			OK = 200,
			BAD_REQUEST = 400,
			NOT_FOUND = 404,
			NOT_ALLOWED = 405,
			NOT_IMPLEMENTED = 501,
			INTERNAL_SERVER_ERROR = 500,
			/* TODO */
		};
	}
}

#endif // RESPONSECODE_H