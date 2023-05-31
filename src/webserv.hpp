/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qjungo <qjungo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 16:23:23 by tplanes           #+#    #+#             */
/*   Updated: 2023/05/31 11:02:48 by tplanes          ###   ########.fr       */
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

#include <cstdlib> // needed for what? forbidden?
#include <errno.h>

# define BACKLOG 25 // number of connections allowed on incoming queue
# define MAX_HEADER_SIZE 2048 // max size of header in received request
//# define MAX_HEADER_SIZE 20 // to test header size error

//# define FD_SETSIZE 1024 // how to overide the default higher value? (should test what happens if pass limit)

#define DEFAULT_CONFIG_FILE_PATH "./config/default.yaml"

# include "server/Client.hpp" // should the class headers be included in the main header ? or the other way around ?

# include "server/HttpResponse.h"
# include "server/HttpRequest.h"
# include "server/request_handler/RequestHandler.h"
# include "server/request_handler/DeleteRequestHandler.h"
# include "server/request_handler/PutRequestHandler.h"
# include "server/request_handler/PostRequestHandler.h"
# include "server/request_handler/GetRequestHandler.h"
# include "http/http.h"

/* Functions ***************************************************/

int		getListenSock(Config *config);

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
