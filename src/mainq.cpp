//
// Created by qjungo on 5/9/23.
//
int main()
{
std::vector<Config *> servers = Config::parse_servers("/home/qjungo/Cursus/webserv/config/default.yaml");
//    servers[0]->log();
//	HttpRequest request("GET /linux_cgi HTTP/1.1\r\nUser-Agent: curl/7.68.0\r\nAccept: */*\r\n\r\n");
HttpRequest request("GET /new_user HTTP/1.1\r\nUser-Agent: curl/7.68.0\r\nAccept: */*\r\n\r\n");
CgiExecutor cgi_executor;
request.log();

std::cout << "======================" << std::endl;
std::string res = cgi_executor.execute(request, *servers[0]);
std::cout << "======================" << std::endl;
std::cout << res << std::endl;
}