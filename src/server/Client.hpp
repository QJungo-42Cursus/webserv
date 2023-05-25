/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qjungo <qjungo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 16:55:07 by tplanes           #+#    #+#             */
/*   Updated: 2023/05/25 18:40:55 by tplanes          ###   ########.fr       */
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
		
		std::string&				getRequest(void);
		std::string&				getHeader(void);
		std::string&				getBody(void);
		
		int							getMaxBodySize(void);
		
		bool						getFlagHeaderComplete(void) const;
		//bool						getFlagOversize(void) const;
		int							getNBytesRec(void) const;
		bool						getFlagResponse(void) const;
		std::string const&			getResponse(void) const;
		bool						getFlagCloseAfterWrite(void) const;

		void						setMaxBodySize(int);
		
		void						setFd(int fd);
		void						setListenFd(int fd);
		void						setNBytesRec(int nBytesRec);
		void						setFlagResponse(bool);
		void						setResponse(std::string);
		void						setFlagCloseAfterWrite(bool);
		
		void						setFlagHeaderComplete(bool);
		//void						setFlagOversize(bool);
		
		void						clearRequest(void);
		void						clearResponse(void);

	private:
		Client(Client const& src);
		Client& operator=(Client const& rhs);

		int						_fd;
		int						_listenFd;
		struct sockaddr_storage	_addr; //_storage big enough for any addr type 
		socklen_t				_addrSize;
		std::string				_request; // the HTTP request
		std::string				_header; // just the header
		std::string				_body; // just the body
		bool					_flagHeaderComplete;
		//bool					_flagOversize; //either header or body	
		int						_maxBodySize;

		int						_nBytesRec;

		bool					_flagResponse; // true when a response is ready to be sent
		std::string				_response;
		
		bool					_flagCloseAfterWrite; // true connection to be closed after writing
};

#endif
