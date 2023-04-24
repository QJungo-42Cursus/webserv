
- la rfc: https://www.rfc-editor.org/rfc/rfc2616
- le sujet: https://cdn.intra.42.fr/pdf/pdf/81824/fr.subject.pdf



ce que jai compris pour le cgi : on doit execve sur un binaire (le cgi) en lui passant envp dans lequel on aura mis les infos de la requete http


- fonctions authorisee:
	- `execve, fork, pipe, dup, dup2` on connait (c'est pour le CGI)
	- `int recv(socket_fd, buf, len, flags)` est un equivalent de read mais pour les sockets ! A utiliser avec poll (ou equivalent)
	- `int send(socket_fd, str, strlen)` meme chose pour write
	- `accept, bind, connect, listen` permettent de connecter un socket (et donc d'ecouter ce qui est envoye sur un port)
	- `strerror, errno`... 
	- The `int gai_error(struct gaicb *req);` function returns the status of the request req: either EAI_INPROGRESS if the request was not completed yet, 0 if it was handled successfully, or an error code if the request could not be resolved.
	- `int poll(struct pollfd *fds, nfds_t nfds, int timeout)` permet d'attendre que le fd du socket aie recu quelque chose. (une sorte d'event listener)