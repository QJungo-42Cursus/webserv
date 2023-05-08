#include <csignal>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <sys/wait.h>
#include "CgiExecutor.h"
#include "../config/Config.h"
#include <cstdlib>
#include <unistd.h>
#include <ctime>

int fork1()
{
	int pid = fork();
	if (pid == -1)
		throw std::exception();// TODO
	return pid;
}


std::string test()
{
	#ifdef __LINUX__
	std::string _path = "cgi/ubuntu_cgi_tester";
	#else
	std::string _path = "cgi/cgi_tester";
	#endif

	_path = "cgi/python_cgi";

	char *const argv[] = {
		(char *) _path.c_str(),
		(char *) "www/index.html",
	 	NULL
	};

	char *const envp[] = {
		(char *) "REQUEST_METHOD=GET", 
		(char *) "PATH_INFO=/",
		// (char *) "PATH_INFO=\"\"",
		(char *) "SERVER_PROTOCOL=HTTP/1.1", 
		(char *) "CONTENT_TYPE=",
		(char *) "CONTENT_LENGTH=",
		(char *) "HTTP_COOKIE=",
		(char *) "HTTP_USER_AGENT=",
		(char *) "QUERY_STRING=",
		(char *) "REMOTE_ADDR=",
		(char *) "REMOTE_HOST=",
		(char *) "SCRIPT_FILENAME=",
		(char *) "SCRIPT_NAME=",
		(char *) "SERVER_NAME=",
		(char *) "SERVER_SOFTWARE=",
		NULL
	};

	int fd_std[2];
	if (pipe(fd_std))
		throw std::exception();
	int fd_err[2];
	if (pipe(fd_err))
		throw std::exception();

	int pid;
	pid = fork();
	if (pid == 0)
	{
		close(fd_std[STDIN_FILENO]);
		close(fd_err[STDIN_FILENO]);
		dup2(fd_std[STDOUT_FILENO], STDOUT_FILENO);
		dup2(fd_err[STDOUT_FILENO], STDERR_FILENO);
		close(fd_std[STDOUT_FILENO]);
		close(fd_err[STDOUT_FILENO]);
		execve(_path.c_str(), argv, envp);
		std::cout << "oups..." << std::endl;
		perror("execvpe: ");
		exit(1);
	}
	close(fd_std[STDOUT_FILENO]);
	close(fd_err[STDOUT_FILENO]);

	const int S_TIMEOUT = 2;

	std::time_t s_start = std::time(0);
	bool did_timeout = false;
	int exit_status = 0;
	while(1)
	{
		std::time_t elapsed = std::time(0) - s_start;
		did_timeout = (elapsed >= S_TIMEOUT);
		if (did_timeout)
			break;
		exit_status = -1;
		waitpid(pid, &exit_status, WNOHANG);
		if (exit_status != -1)
		{
			exit_status = WEXITSTATUS(exit_status);
			break;
		}
		usleep(10000);
	}
	if (did_timeout)
	{
		// TODO kill le process
		std::cout << "TIMEOUT !! (the waitpid on the lauched CGI wait for too long)" << std::endl;
		return "";
	}

	std::cout << "exit status: " << exit_status << std::endl;

	if (exit_status == 0)
	{
		char buff[2048] = { 0 };
		int count = read(fd_std[STDIN_FILENO], buff, 2047);
		buff[count] = 0;
		std::cout << "buff: '" << buff << "'" << std::endl;
	}
	else 
	{
		char buff[2048] = { 0 };
		int count = read(fd_err[STDIN_FILENO], buff, 2047);
		buff[count] = 0;
		std::cout << "ebuff: '" << buff << "'" << std::endl;
	}
	close(fd_err[STDIN_FILENO]);
	close(fd_std[STDIN_FILENO]);
	return "";
}



std::map<std::string, std::string> get_env(const HttpRequest &request)
{
	std::map<std::string, std::string> env;

	if (request.get_headers().count("content-type"))
		env["CONTENT_TYPE"] = request.get_headers().find("content-type")->second;
	if (request.get_headers().count("content-length"))
		env["CONTENT_LENGTH"] = request.get_headers().find("content-length")->second;
	if (request.get_headers().count("cookie"))
		env["HTTP_COOKIE"] = request.get_headers().find("cookie")->second;
	if (request.get_headers().count("user-agent"))
		env["HTTP_USER_AGENT"] = request.get_headers().find("user-agent")->second;
	if (request.get_headers().count("query-string"))
		env["QUERY_STRING"] = request.get_headers().find("query-string")->second;
	if (request.get_headers().count("remote-addr"))
		env["REMOTE_ADDR"] = request.get_headers().find("remote-addr")->second;
	if (request.get_headers().count("remote-host"))
		env["REMOTE_HOST"] = request.get_headers().find("remote-host")->second;
	if (request.get_headers().count("script-filename"))
		env["SCRIPT_FILENAME"] = request.get_headers().find("script-filename")->second;
	if (request.get_headers().count("script-name"))
		env["SCRIPT_NAME"] = request.get_headers().find("script-name")->second;
	if (request.get_headers().count("server-name"))
		env["SERVER_NAME"] = request.get_headers().find("server-name")->second;
	if (request.get_headers().count("server-software"))
		env["SERVER_SOFTWARE"] = request.get_headers().find("server-software")->second;
	if (request.get_headers().count("request-method"))
		env["REQUEST_METHOD"] = request.get_headers().find("request-method")->second;
	if (request.get_headers().count("path-info"))
		env["PATH_INFO"] = request.get_headers().find("path-info")->second;
	if (request.get_headers().count("server-protocol"))
		env["SERVER_PROTOCOL"] = request.get_headers().find("server-protocol")->second;
	if (request.get_headers().count("http-accept"))
		env["HTTP_ACCEPT"] = request.get_headers().find("http-accept")->second;
	if (request.get_headers().count("http-accept-charset"))
		env["HTTP_ACCEPT_CHARSET"] = request.get_headers().find("http-accept-charset")->second;
	if (request.get_headers().count("http-accept-encoding"))
		env["HTTP_ACCEPT_ENCODING"] = request.get_headers().find("http-accept-encoding")->second;
	if (request.get_headers().count("http-accept-language"))
		env["HTTP_ACCEPT_LANGUAGE"] = request.get_headers().find("http-accept-language")->second;
	if (request.get_headers().count("http-connection"))
		env["HTTP_CONNECTION"] = request.get_headers().find("http-connection")->second;
	if (request.get_headers().count("http-host"))
		env["HTTP_HOST"] = request.get_headers().find("http-host")->second;
	// TODO check if we need to add more headers, or if some are not existing
	return env;
}

std::string CgiExecutor::execute(const HttpRequest &request, const Config &config)
{
    // log request _headers

    for (std::map<std::string, std::string>::iterator it = request.get_headers().begin(); it != request.get_headers().end(); it++)
        std::cout << it->first << ": " << it->second << std::endl;

    std::map<std::string, std::string> env = get_env(request);
    for (std::map<std::string, std::string>::iterator it = env.begin(); it != env.end(); ++it)
    {
        std::cout << it->first << ": " << it->second << std::endl;
    }

    return "";
}