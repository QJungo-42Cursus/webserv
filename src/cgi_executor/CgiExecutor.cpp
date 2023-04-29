#include <csignal>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <sys/wait.h>
#include "CgiExecutor.h"

int fork1()
{
	int pid = fork();
	if (pid == -1)
		throw std::exception();// TODO
	return pid;
}


std::string CgiExecutor::execute()
{
	std::string _path = "cgi/ubuntu_cgi_tester";
	char *const argv[] = {(char *) _path.c_str(), (char *) "www/index.html",
						  NULL};
	//https://fr.wikipedia.org/wiki/Variables_d%27environnement_CGI
	char *const envp[] = {(char *) "REQUEST_METHOD=GET", (char *) "SERVER_PROTOCOL=HTTP/1.1", (char *) "PATH_INFO=www/",
						  NULL};

	int fd[2];
	if (pipe(fd))
		throw std::exception();

	if (fork() == 0)
	{
		std::cout << "child" << std::endl;
		close(fd[STDIN_FILENO]);
		dup2(fd[STDOUT_FILENO], STDOUT_FILENO);
		close(fd[STDOUT_FILENO]);
		execvpe(_path.c_str(), argv, envp);
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
