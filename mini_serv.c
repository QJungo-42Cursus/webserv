#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FD FD_SETSIZE
#define BUFF 1024 * 42

int ids[MAX_FD];
int last_id = 0;

void send_all(char *message, int main_fd, int sender_fd)
{
	for (int fd = 3; fd < MAX_FD; fd++)
	{
		if (fd == main_fd || fd == sender_fd)
			continue;
		send(fd, message, strlen(message), 0);
	}
}

void crash() {
	write(2, "Fatal error\n", strlen("Fatal error\n"));
	exit(1);
}

int main(int argc, char **argv)
{
	argc--;
	argv++;

	if (argc != 1)
	{
		printf("Wrong number of arguments\n");
		return (1);
	}
	int port = atoi(argv[0]);

	// socket create and verification
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
		crash();

	// assign IP, PORT
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433); // 127.0.0.1
	servaddr.sin_port = htons(port);

	// Binding newly created socket to given IP and verification
	if ((bind(server_fd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
		crash();
	if (listen(server_fd, 10) != 0)
		crash();

	fd_set set_main, set_read;
	FD_ZERO(&set_main);
	FD_SET(server_fd, &set_main);

	int last_was_no_newline = 0;

	while (1)
	{
		set_read = set_main;
		select(MAX_FD, &set_read, NULL,  NULL, NULL);

		for (int fdesc = 3; fdesc < MAX_FD; fdesc++)
		{
			if (!FD_ISSET(fdesc, &set_read))
				continue;
			
			if (fdesc == server_fd)
			{
				int client_fd = accept(server_fd, NULL, 0);
				if (client_fd < 0) {
					printf("opus23\n");
					exit(0);
				}
				char msg[100];
				sprintf(msg, "server: client %d just arrived\n", last_id);
				ids[client_fd] = last_id;
				last_id ++;
				FD_SET(client_fd, &set_main);
				send_all(msg, server_fd, client_fd);
				break;
			}
			
			char buff[BUFF];
			int buff_len = recv(fdesc, buff, BUFF, 0);
			if (buff_len <= 0)
			{
				char msg[50];
				sprintf(msg, "server: client %d just left\n", ids[fdesc]);
				send_all(msg, server_fd, fdesc);
				FD_CLR(fdesc, &set_main);
				close(fdesc);
				break;
			}
			buff[buff_len] = 0;
			char last_char = buff[buff_len - 1];

			int i = 0;
			int last_start = 0;
			while (buff[i]) {
				if (buff[i] == '\n') {
					buff[i] = 0;
					char msg[BUFF];
					sprintf(msg, "client %d: %s\n", ids[fdesc], &buff[last_start]);
					last_start = i + 1;
					send_all(msg, server_fd, fdesc);
				}
				i += 1;
			}
			if (last_start < buff_len) {
				char msg[BUFF];
				if (last_was_no_newline) {
					sprintf(msg, "%s", &buff[last_start]);
				} else {
					sprintf(msg, "client %d: %s", ids[fdesc], &buff[last_start]);
				}	
				send_all(msg, server_fd, fdesc);
			}
			if (last_char != '\n') {
				last_was_no_newline = 1;
			}
		}
	}
}
