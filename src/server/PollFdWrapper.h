#ifndef POLL_FD_WRAPPER
#define POLL_FD_WRAPPER
#include <poll.h>

class PollFdWrapper {
public:
	PollFdWrapper(int fd, short flags);


	// only for revents
	bool isErr();
	bool isHup();
	bool isNVal();

private:
	PollFdWrapper();
	struct pollfd _pollfd;
};

#endif

/*
 * Requestable events.  If poll(2) finds any of these set, they are
 * copied to revents on return.
 
#define POLLIN          0x0001          /* any readable data available 
#define POLLPRI         0x0002          /* OOB/Urgent readable data 
#define POLLOUT         0x0004          /* file descriptor is writeable 
#define POLLRDNORM      0x0040          /* non-OOB/URG data available 
#define POLLWRNORM      POLLOUT         /* no write type differentiation 
#define POLLRDBAND      0x0080          /* OOB/Urgent readable data 
#define POLLWRBAND      0x0100          /* OOB/Urgent data can be written 

 * These events are set if they occur regardless of whether they were
 * requested.
#define POLLERR         0x0008          /* some poll error occurred 
#define POLLHUP         0x0010          /* file descriptor was "hung up" 
#define POLLNVAL        0x0020          /* requested events "invalid" 
 */