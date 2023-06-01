/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tplanes <tplanes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 18:22:48 by tplanes           #+#    #+#             */
/*   Updated: 2023/05/26 09:30:24 by tplanes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(void) : _fd(-1), _addrSize(sizeof(this->_addr)), _nBytesRec(0),
	_flagResponse(false), _flagCloseAfterWrite(false), _flagHeaderComplete(false)
{
	std::cout << "Client default constructor called" << std::endl;
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

std::string&	Client::getRequest(void)
{
	return (this->_request);
}

std::string&	Client::getHeader(void)
{
	return (this->_header);
}

std::string&	Client::getBody(void)
{
	return (this->_body);
}

int				Client::getMaxBodySize(void)
{
	return (this->_maxBodySize);
}

bool			Client::getFlagHeaderComplete(void) const
{
	return (this->_flagHeaderComplete);
}

/*bool			Client::getFlagOversize(void)
{
	return (this->_flagOversize);
}*/

int	Client::getNBytesRec(void) const
{
	return (this->_nBytesRec);
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

void	Client::clearRequest(void)
{
	(this->getRequest()).clear();
	return ;
}

void	Client::clearHeader(void)
{
	(this->getHeader()).clear();
	return ;
}

void	Client::clearBody(void)
{
	(this->getBody()).clear();
	return ;
}

void	Client::setFlagHeaderComplete(bool flag)
{
	this->_flagHeaderComplete = flag;
	return ;
}

void	Client::setMaxBodySize(int size)
{
	this->_maxBodySize = size;
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

void	Client::setNBytesRec(ssize_t nBytesRec)
{
	this->_nBytesRec = nBytesRec;
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

