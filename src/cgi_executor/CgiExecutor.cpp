#include <csignal>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <sys/wait.h>
#include "CgiExecutor.h"
#include <cstdlib>
#include <unistd.h>

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
	// std::string _path = "cgi/python_cgi";

	char *const argv[] = {
		(char *) _path.c_str(),
		(char *) "www/index.html",
	 	NULL
	};

	char *const envp[] = {
		(char *) "REQUEST_METHOD=GET", 
		(char *) "PATH_INFO=index.html",
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

	int fd[2];
	if (pipe(fd))
		throw std::exception();

	if (fork() == 0)
	{
		std::cout << "child" << std::endl;
		close(fd[STDIN_FILENO]);
		dup2(fd[STDOUT_FILENO], STDOUT_FILENO);
		close(fd[STDOUT_FILENO]);
		execve(_path.c_str(), argv, envp);
		std::cout << "oups..." << std::endl;
		perror("execvpe: ");
		exit(1);
	}
	close(fd[STDOUT_FILENO]);
	wait(NULL);
	char buff[2048];
	int count = read(fd[STDIN_FILENO], buff, 2047);
	buff[count] = 0;
	close(fd[STDIN_FILENO]);

	std::cout << "buff: '" << buff << "'" << std::endl;

	return std::string();
}
