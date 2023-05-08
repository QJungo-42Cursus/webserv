#include <csignal>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <sys/wait.h>
#include "CgiExecutor.h"
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


std::string CgiExecutor::execute()
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
