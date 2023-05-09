#include <vector>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cstdio>
#include <sys/wait.h>
#include "CgiExecutor.h"
#include <unistd.h>
#include <ctime>
#include <sstream>

static int fork1()
{
	int pid = fork();
	if (pid == -1)
		throw std::exception();// TODO
	return pid;
}

static std::map<std::string, std::string> get_env(const HttpRequest &request, const Config &config)
{
	std::map<std::string, std::string> env;
	std::map<std::string, std::string> headers = request.get_headers();

	/// Server related
	env["SERVER_SOFTWARE"] = "webserv/1.0";
	env["SERVER_NAME"] = config.server_name.isSome() ? config.server_name.unwrap() : "localhost";
	env["GATEWAY_INTERFACE"] = "CGI/1.1";

	/// request related
	env["SERVER_PROTOCOL"] = request.get_version();
	env["SERVER_PORT"] = "80";
	if (config.port.isSome())
	{
		std::stringstream port("");
		port << config.port.unwrap();
		env["SERVER_PORT"] = port.str();
	}
	int method = request.get_method();
	if (method | Http::Methods::GET)
		env["REQUEST_METHOD"] = "GET";
	else if (method | Http::Methods::POST)
		env["REQUEST_METHOD"] = "POST";
	else if (method | Http::Methods::DELETE)
		env["REQUEST_METHOD"] = "DELETE";
//	env["PATH_INFO"] = request.get_path(); // TODO pas sur
	env["PATH_INFO"] = "cgi"; // TODO pas sur
	env["PATH_TRANSLATED"] = request.get_path(); // TODO pas sur
	env["SCRIPT_NAME"] = config.routes.find(request.get_path())->second->cgi.unwrap().cgi_path; // TODO pas sur
//	env["QUERY_STRING"] // TODO add query string
	if (headers.count("Host"))
		env["REMOTE_HOST"] = headers["Host"]; // TODO pas sur
	else
		env["REMOTE_HOST"] = "";
	if (headers.count("Host"))
		env["REMOTE_ADDR"] = headers["Host"]; // TODO pas sur
	else
		env["REMOTE_ADDR"] = "";
	env["AUTH_TYPE"] = "";
	env["REMOTE_USER"] = "";
	env["REMOTE_IDENT"] = "";
	env["CONTENT_TYPE"] = headers.count("Content-Type") ? headers["Content-Type"] : "";
	env["CONTENT_LENGTH"] = headers.count("Content-Length") ? headers["Content-Length"] : "";

	/// client related
	env["HTTP_ACCEPT"] = headers.count("Accept") ? headers["Accept"] : "";
	env["HTTP_ACCEPT_LANGUAGE"] = headers.count("Accept-Language") ? headers["Accept-Language"] : "";
	env["HTTP_USER_AGENT"] = headers.count("User-Agent") ? headers["User-Agent"] : "";
	env["HTTP_COOKIE"] = headers.count("Cookie") ? headers["Cookie"] : "";
	env["HTTP_REFERER"] = headers.count("Referer") ? headers["Referer"] : "";

	return env;
}

static char **map_to_env(std::map<std::string, std::string> env)
{
	char **envp = new char *[env.size() + 1];
	int i = 0;
	for (std::map<std::string, std::string>::iterator it = env.begin(); it != env.end(); ++it)
	{
		envp[i] = new char[it->second.size() + it->first.size() + 2];
		envp[i] = strcpy(envp[i], (it->first + "=" + it->second).c_str());
		i++;
	}
	envp[i] = NULL;
	return envp;
}

static void free_env(char **envp)
{
	int i = 0;
	while (envp[i])
	{
		delete[] envp[i];
		i++;
	}
	delete[] envp;
}

std::string CgiExecutor::execute(const HttpRequest &request, const Config &config)
{
	std::map<std::string, std::string> env = get_env(request, config);
	char **envp = map_to_env(env);
	std::string cgi_path = config.routes.find(request.get_path())->second->cgi.unwrap().cgi_path;
	char *const argv[] = {
			(char *) cgi_path.c_str(),
			(char *) request.get_path().c_str(),
			NULL
	};

	std::cout << "path: " << cgi_path << std::endl;
	std::cout << "path_info: " << env["PATH_INFO"] << std::endl;

	int fd_std[2];
	if (pipe(fd_std))
		throw std::exception();
	int fd_err[2];
	if (pipe(fd_err))
		throw std::exception();

	int pid;
	pid = fork1();
	if (pid == 0)
	{
		close(fd_std[STDIN_FILENO]);
		close(fd_err[STDIN_FILENO]);
		dup2(fd_std[STDOUT_FILENO], STDOUT_FILENO);
		dup2(fd_err[STDOUT_FILENO], STDERR_FILENO);
		close(fd_std[STDOUT_FILENO]);
		close(fd_err[STDOUT_FILENO]);
		execve(cgi_path.c_str(), argv, envp);
		std::cout << "oups..." << std::endl;
		perror("execvpe: ");
		free_env(envp);
		exit(1);
	}
	close(fd_std[STDOUT_FILENO]);
	close(fd_err[STDOUT_FILENO]);
	free_env(envp);

	const int S_TIMEOUT = 2;

	std::time_t s_start = std::time(0);
	bool did_timeout = false;
	int exit_status = 0;
	while (true)
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

	std::string res;
	if (exit_status == 0)
	{
		char buff[2048] = {0};
		int count = read(fd_std[STDIN_FILENO], buff, 2047);
		buff[count] = 0;
		res = buff;
	}
	else
	{
		char buff[2048] = {0};
		int count = read(fd_err[STDIN_FILENO], buff, 2047);
		buff[count] = 0;
		res = buff;
	}
	close(fd_err[STDIN_FILENO]);
	close(fd_std[STDIN_FILENO]);
	return res;
}