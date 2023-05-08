/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qjungo <qjungo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 16:23:23 by tplanes           #+#    #+#             */
/*   Updated: 2023/05/08 13:45:16 by qjungo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_H
# define WEBSERV_H

# include <sys/socket.h>
# include <sys/types.h>
# include <netdb.h>
# include <sys/select.h>

# include <fcntl.h>
# include <unistd.h>

# include <cstring>
# include <string>
# include <iostream>
# include <sstream>
# include <fstream>

# define PORT "8080"
# define BACKLOG 5 // number of connections allowed on incoming queue
# define BUFFSIZE 2048 // size of read buffer containing request

//# define FD_SETSIZE 1024 // how to overide the default higher value? (should test what happens if pass limit)

# include "server/Client.hpp"

/* Functions ***************************************************/

int	getListenSock(void);

/* Structures *************************************************/

/* Set of fds to be used with select()
   read, write, and error are tmp copies of main to be modified by select
   (select will check nfds = fdMax + 1 values) */
typedef struct s_fdSets
{
	fd_set	main;
	fd_set	read;
	fd_set	write;
	fd_set	error;
	int		fdMax;
}	t_fdSets;

#endif
