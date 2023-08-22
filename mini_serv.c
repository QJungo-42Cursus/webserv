// ======= V2 ======

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFF 2048

fd_set set_main, set_read, set_write;
int bigger_fd = 0;

void crash()
{
	write(2, "Fatal error\n", strlen("Fatal error\n"));
	exit(1);
}

void send_all(char *message, int sender, int server)
{
	for (int fds = 3; fds < bigger_fd; fds++) {
		if (fds != sender && fds != server && FD_ISSET(fds, &set_write))
			send(fds, message, strlen(message), 0);
	}
}

int main(int argc, char **argv) {
	if (argc != 2) {
		write(2, "Wrong number of arguments\n", strlen("Wrong number of arguments\n"));
		exit(1);
	}

	// socket create and verification
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
		crash();

	// assign IP, PORT
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(atoi(argv[1]));

	// Binding newly created socket to given IP and verification
	if ((bind(server_fd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) < 0)
		crash();
	if (listen(server_fd, 10) < 0)
		crash();


	FD_ZERO(&set_main);
	FD_SET(server_fd, &set_main);
	int ids[1050];
	int last_id = 0;
	int last_msg_no_nl = 0;

	bigger_fd = server_fd + 10;

	while (1) {
		FD_COPY(&set_main, &set_read);
		FD_COPY(&set_main, &set_write);
		select(bigger_fd, &set_read, &set_write, NULL, NULL);
		for (int current_fd = 3; current_fd < bigger_fd; current_fd++) {
			if (!FD_ISSET(current_fd, &set_read))
				continue;
			if (current_fd == server_fd) {
				int new = accept(current_fd, NULL, 0);
				if (new < 0)
					crash();
				if (bigger_fd < new)
					bigger_fd = new + 10;
				FD_SET(new, &set_main);
				char msg[90];
				sprintf(msg, "server: client %d just arrived\n", last_id);
				send_all(msg, new, server_fd);
				last_msg_no_nl = 0;
				ids[new] = last_id;
				last_id++;
				break;
			}
			char buffer[BUFF + 5];
			bzero(buffer, BUFF + 5);
			int len = recv(current_fd, buffer, BUFF, 0);
			if (len <= 0) {
				char msg[90];
				sprintf(msg, "server: client %d just left\n", ids[current_fd]);
				send_all(msg, current_fd, server_fd);
				last_msg_no_nl = 0;
				FD_CLR(current_fd, &set_main);
				close(current_fd);
				break;
			}

			char last_char = buffer[len - 1];
			// printf("--- '%c' ---\n", last_char);
			int i = 0;
			int last_index = 0;

			while (buffer[i]) {
				if (buffer[i] == '\n') {
					// printf("msg !\n");
					buffer[i] = 0;
					char msg[BUFF + 200];
					if (last_msg_no_nl) {
						sprintf(msg, "%s\n", &buffer[last_index]);
					} else {
						sprintf(msg, "client %d: %s\n", ids[current_fd], &buffer[last_index]);
					}
					last_msg_no_nl = 0;
					send_all(msg, current_fd, server_fd);
					last_index = i + 1;
				}
				i++;
			}

			if ((last_index == 0) || (buffer[i] == 0 && last_char != '\n')) {
				// printf("last is no nl\n");
				char msg[BUFF + 200];
				if (last_msg_no_nl) {
					sprintf(msg, "%s", &buffer[last_index]);
				} else {
					sprintf(msg, "client %d: %s", ids[current_fd], &buffer[last_index]);
				}
				last_msg_no_nl = 1;
				send_all(msg, current_fd, server_fd);
			}
			// printf("\n");
		}
	}
}

// ================= V1 ===========

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
