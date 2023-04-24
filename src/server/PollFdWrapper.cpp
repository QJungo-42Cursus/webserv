#include "PollFdWrapper.h"
#include <exception>

PollFdWrapper::PollFdWrapper(int fd, short flags)
{
	_pollfd.fd = fd;
	_pollfd.events = flags;
}

void PollFdWrapper::poll(int timeout)
{
	if (::poll(&_pollfd, 1, timeout) == -1)
	{
		throw std::exception();
	}
	if (isErr() || isHup() || isNVal())
	{
		// TODO : handle error more precisely
		throw std::exception();
	}
}

bool PollFdWrapper::isErr() const { return POLLERR & _pollfd.revents; }
bool PollFdWrapper::isHup() const { return POLLHUP & _pollfd.revents; }
bool PollFdWrapper::isNVal() const { return POLLNVAL & _pollfd.revents; }
bool PollFdWrapper::isRdNorm() const { return POLLRDNORM & _pollfd.revents; }
bool PollFdWrapper::isRdBand() const { return POLLRDBAND & _pollfd.revents; }
bool PollFdWrapper::isWrNorm() const { return POLLWRNORM & _pollfd.revents; }
bool PollFdWrapper::isWrBand() const { return POLLWRBAND & _pollfd.revents; }
bool PollFdWrapper::isPri() const { return POLLPRI & _pollfd.revents; }
bool PollFdWrapper::isOut() const { return POLLOUT & _pollfd.revents; }
bool PollFdWrapper::isIn() const { return POLLIN & _pollfd.revents; }
