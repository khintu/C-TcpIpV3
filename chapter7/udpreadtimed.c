#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

int errexit(const char *format, ...);

/*
Sets the timeout value that specifies the maximum amount of time an input
function waits until it completes. It accepts a timeval structure with the
number of seconds and microseconds specifying the limit on how long to wait
for an input operation to complete. If a receive operation has blocked for
this much time without receiving additional data, it shall return with a 
partial count or errno set to [EAGAIN] or [EWOULDBLOCK] if no data is received.
The default for this option is zero, which indicates that a receive operation
shall not time out. This option takes a timeval structure. Note that not all
implementations allow this option to be set.
*/

int readUdpTimed(int s, void* buf, int buflen, time_t timeout_in_sec)
{
	int n;
	struct timeval tv;
	tv.tv_sec = timeout_in_sec;
	tv.tv_usec = 0;
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);	
	
	if ((n = read(s, buf, buflen)) < 0) 
	{
		if (errno != EAGAIN && errno != EWOULDBLOCK)
			errexit("Could not read from peer: %s\n", strerror(errno));
		else
			errexit("Read timed out\n");
	}
	return n;
}

int readUdpTimed2(int s, void* buf, int buflen, time_t timeout_in_sec)
{
	int n;
	struct timeval tv;
	tv.tv_sec = timeout_in_sec;
	tv.tv_usec = 0;
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);	
	
	if ((n = read(s, buf, buflen)) < 0) 
	{
		if (errno != EAGAIN && errno != EWOULDBLOCK)
		{
			printf("Could not read from peer: %s\n", strerror(errno));
			return -1;
		}
		else {
			printf("Read timed out\n");
			return -2;
		}
	}
	return n;
}
