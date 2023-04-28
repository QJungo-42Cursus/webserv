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
	std::string _path = "./cgi/";
	char *const argv[2] = {(char *) _path.c_str(), NULL};

	int fd[2];
	if (pipe(fd))
		throw std::exception();

	if (fork() == 0)
	{
		close(fd[STDIN_FILENO]);
		dup2(fd[STDOUT_FILENO], STDOUT_FILENO);
		close(fd[STDOUT_FILENO]);
		execvpe(_path.c_str(), argv, NULL);
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
