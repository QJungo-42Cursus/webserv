#include <iostream>
#include "cgi_executor/CgiExecutor.h"


#ifdef _TEST_
int _main(int argc, char *argv[])
#else

int main()
#endif
{
    std::vector<Config *> servers = Config::parse_servers("/home/qjungo/Cursus/webserv/config/default.yaml");
//    servers[0]->log();
    HttpRequest request("GET / HTTP/1.1\r\nHost: localhost:8080\r\nUser-Agent: curl/7.68.0\r\nAccept: */*\r\n\r\n");
    CgiExecutor cgi_executor;
    std::cout << cgi_executor.execute(request, *servers[0]) << std::endl;
}