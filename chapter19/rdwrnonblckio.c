#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>


int errexit(const char *,...);

int sendnonblk(int s, void *buf, int nchars)
{
	int n;
	while (1) {
		if ((n = send(s, buf, nchars, MSG_DONTWAIT)) < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				usleep(300);
				continue;
			}
			else
				errexit("Send error: %s\n", strerror(errno));
		}
		break;
	}
	return n;
}

/*
	Internet lifetime for a packet is of the order of millisec.
	So, we are not going to wait for more than a few millisec, in 
	this case 15 millisec
*/
int recvnonblk(int s, void* buf, int nchars)
{
	int n, retry = 5;
	while (retry) {
		if ((n = recv(s, buf, nchars, MSG_DONTWAIT)) < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				if (retry < 5)
					printf("Recevie retry...\n");
				usleep(3000);
				retry--;
				continue;
			}
			else
				errexit("Receive error: %s\n", strerror(errno));
		}
		break;
	}
	if (retry < 0)
		return -1;
	return n;
}


int sendNBytes(int s, void *buf, int nchars)
{
	int cc = 0;
	int idx = 0;
	while (nchars && (cc = sendnonblk(s, ((unsigned char*)buf)+idx, nchars)) > 0)
			nchars-=cc,idx+=cc;
	if (cc <= 0)
		return cc;
	else if (nchars == 0)
		return idx;
	else
		return -1;
}

int recvNBytes(int s, void *buf, int nchars)
{
	int cc = 0;
	int idx = 0;
	while (nchars && (cc = recvnonblk(s, ((unsigned char*)buf)+idx, nchars)) > 0)
			nchars-=cc,idx+=cc;
	if (cc <= 0)
		return cc;
	else if (nchars == 0)
		return idx;
	else
		return -1;
}
