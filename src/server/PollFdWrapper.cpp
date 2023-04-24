#include "PollFdWrapper.h"

PollFdWrapper::PollFdWrapper(int fd, short flags)
{
	_pollfd.fd = fd;
	_pollfd.events = flags;
}

bool PollFdWrapper::isErr(){	return POLLERR & _pollfd.revents;}

bool PollFdWrapper::isHup(){	return POLLHUP & _pollfd.revents;}

bool PollFdWrapper::isNVal(){	return POLLNVAL & _pollfd.revents;}