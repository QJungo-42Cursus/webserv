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
#include "../http/request_handler/RequestHandler.h"

static int fork1()
{
	int pid = fork();
	if (pid == -1)
		throw std::runtime_error("fork failed");
	return pid;
}

static std::map<std::string, std::string> get_env(const HttpRequest &request, const Config &config, const Route &route)
{
	std::map<std::string, std::string> env;
	std::map<std::string, std::string> headers = request.get_headers();

	/// Server related
	env["SERVER_SOFTWARE"] = "webserv/1.0";
	env["SERVER_NAME"] = config.server_name.isSome() ? config.server_name.unwrap() : "localhost";
	env["GATEWAY_INTERFACE"] = "CGI/1.1";

	/// request related
	env["SERVER_PROTOCOL"] = request.get_version();
	std::stringstream portss;
	portss << config.port;
	env["SERVER_PORT"] = portss.str();
	if (config.port)
	{
		std::stringstream port("");
		port << config.port;
		env["SERVER_PORT"] = port.str();
	}
	int method = request.get_method();
	if (method == Http::Methods::GET)
		env["REQUEST_METHOD"] = "GET";
	else if (method == Http::Methods::POST)
		env["REQUEST_METHOD"] = "POST";
	else if (method == Http::Methods::DELETE)
		env["REQUEST_METHOD"] = "DELETE";
	else if (method == Http::Methods::PUT)
		env["REQUEST_METHOD"] = "PUT";
	env["PATH_INFO"] = request.get_path();
	env["SCRIPT_NAME"] = real_path(route, request);
	env["REMOTE_HOST"] = "";
	if (headers.count("Host"))
		env["REMOTE_HOST"] = headers["Host"];
	env["REMOTE_USER"] = headers.count("Host") ? headers["Host"] : "";
	env["CONTENT_TYPE"] = headers.count("Content-Type") ? headers["Content-Type"] : "";
	env["CONTENT_LENGTH"] = headers.count("Content-Length") ? headers["Content-Length"] : "";
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


std::string
CgiExecutor::execute(const HttpRequest &request, const Config &config, const CgiConfig &cgi_config, const Route &route)
{
	/// Make env
	std::map<std::string, std::string> env = get_env(request, config, route);
	char **envp = map_to_env(env);

	/// Make argv
	std::string request_path = real_path(route, request);
	char *const argv[] = {
			new char[cgi_config.cgi_path.size() + 1],
			new char[request_path.size() + 1],
			NULL
	};
	strcpy(argv[0], cgi_config.cgi_path.c_str());
	argv[0][cgi_config.cgi_path.size()] = '\0';
	strcpy(argv[1], request_path.c_str());
	argv[1][request_path.size()] = '\0';
	if (access(argv[1], F_OK) == -1)
		throw std::runtime_error("file not found");
	if (access(argv[0], X_OK) == -1)
		throw std::runtime_error("file not executable");

	/// Make pipes
	int fd_std[2];
	if (pipe(fd_std))
		throw std::runtime_error("pipe failed");
	int fd_in[2];
	if (pipe(fd_in))
		throw std::runtime_error("pipe failed");

	/// Execute
	int pid;
	pid = fork1();
	if (pid == 0)
	{
		close(fd_in[STDOUT_FILENO]);
		close(fd_std[STDIN_FILENO]);
		dup2(fd_in[STDIN_FILENO], STDIN_FILENO);
		close(fd_in[STDIN_FILENO]);
		dup2(fd_std[STDOUT_FILENO], STDOUT_FILENO);
		close(fd_std[STDOUT_FILENO]);
		execve(cgi_config.cgi_path.c_str(), argv, envp);
		perror("execvpe: ");
		free_env(envp);
		exit(1);
	}

	/// Write to stdin
	write(fd_in[STDOUT_FILENO], request.get_body().c_str(), request.get_body().size());

	/// Close pipes and free memory
	close(fd_in[STDOUT_FILENO]);
	close(fd_in[STDIN_FILENO]);
	close(fd_std[STDOUT_FILENO]);
	free_env(envp);

	/// Wait for child and check for timeout
	const int S_TIMEOUT = 2;
	std::time_t s_start = std::time(NULL);
	bool did_timeout;
	int exit_status = 0;
	while (true)
	{
		std::time_t elapsed = std::time(NULL) - s_start;
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
		throw std::runtime_error("CGI timeout");
	if (exit_status != 0)
		throw std::runtime_error("CGI exit with status not 0");

	/// Read from stdout
	char buff[2048] = {0};
	std::size_t count = read(fd_std[STDIN_FILENO], buff, 2047);
	buff[count] = 0;
	close(fd_std[STDIN_FILENO]);
	return buff;
}