/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tplanes <tplanes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 18:22:48 by tplanes           #+#    #+#             */
/*   Updated: 2023/05/09 15:24:51 by tplanes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

// Warning, shouldnt use accessor for _addr here (possible cast issue)
Client::Client(void) : _fd(-1), _addrSize(sizeof(this->_addr)), _nBytesRequest(0),
	_flagResponse(false), _flagCloseAfterWrite(false) 
{
	std::cout << "Client default constructor called" << std::endl;
	this->clearRequestBuff();
	return ;
}

Client::~Client(void)
{
	std::cout << "Client destructor called for sock fd = " << this->getFd() << std::endl;
	return ;
}

/*Getters/Setters**********************/

int	Client::getFd(void) const
{
	return (this->_fd);
}

int	Client::getListenFd(void) const
{
	return (this->_listenFd);
}

char*	Client::getRequestBuff(void)
{
	return (this->_request);
}

int	Client::getNBytesRequest(void) const
{
	return (this->_nBytesRequest);
}

struct sockaddr*	Client::getAddr(void)
{
	return ((struct sockaddr*)&this->_addr);
}

socklen_t*	Client::getAddrSize(void)
{
	return (&this->_addrSize);
}

bool	Client::getFlagResponse(void) const
{
	return (this->_flagResponse);
}
		
std::string const&	Client::getResponse(void) const
{
	return (this->_response);
}

bool	Client::getFlagCloseAfterWrite(void) const
{
	return (this->_flagCloseAfterWrite);
}

void	Client::clearRequestBuff(void)
{
	memset(this->getRequestBuff(), 0, BUFFSIZE);
	return ;
}

void	Client::setFd(int fd)
{
	this->_fd = fd;
	return ;
}

void	Client::setListenFd(int fd)
{
	this->_listenFd = fd;
	return ;
}

void	Client::setNBytesRequest(int nBytesRequest)
{
	this->_nBytesRequest = nBytesRequest;
	return ;
}

void	Client::setFlagResponse(bool flag)
{
	this->_flagResponse = flag;
	return ;
}

void	Client::setResponse(std::string response)
{
	this->_response = response;
	return ;
}

void	Client::clearResponse(void)
{
	(this->_response).clear();
	return ;
}

void	Client::setFlagCloseAfterWrite(bool flag)
{
	this->_flagCloseAfterWrite = flag;
	return ;
}

/*******************************/

