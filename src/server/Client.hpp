/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qjungo <qjungo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 16:55:07 by tplanes           #+#    #+#             */
/*   Updated: 2023/05/09 15:23:51 by tplanes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_H
# define CLIENT_H

#include "../webserv.hpp"

class Client
{
	public:
		Client(void);
		~Client(void);

		int							getFd(void) const;
		int							getListenFd(void) const;
		struct sockaddr*			getAddr(void);
		socklen_t*					getAddrSize(void);
		char*						getRequestBuff(void);
		int							getNBytesRequest(void) const;
		bool						getFlagResponse(void) const;
		std::string const&			getResponse(void) const;
		bool						getFlagCloseAfterWrite(void) const;

		void						setFd(int fd);
		void						setListenFd(int fd);
		void						setNBytesRequest(int nBytesRequest);
		void						setFlagResponse(bool);
		void						setResponse(std::string);
		void						setFlagCloseAfterWrite(bool);
		
		void						clearRequestBuff(void);
		void						clearResponse(void);

	private:
		Client(Client const& src);
		Client& operator=(Client const& rhs);

		int						_fd;
		int						_listenFd;
		struct sockaddr_storage	_addr; //_storage big enough for any addr type 
		socklen_t				_addrSize;
		char					_request[BUFFSIZE]; // contains the HTTP request
		int						_nBytesRequest;

		bool					_flagResponse; // true when a response is ready to be sent
		std::string				_response;
		
		bool					_flagCloseAfterWrite; // true connection to be closed after writing
};

#endif
