#include "cgi_executor/CgiExecutor.h"



#ifdef _TEST_
int _main(int argc, char *argv[])
#else
int main()
#endif
{
	CgiExecutor cgi_executor;
	cgi_executor.execute();
}