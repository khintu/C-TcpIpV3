#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define QLEN	5
#define BUFSIZE	4096


int errexit(const char *format, ...);
int passiveTCP(const char *service, const int qlen);
int echo(int);

/* Concurrent TCP server for ECHO service */

int main (int argc, char* argv[]) 
{
	char *service = "echo";
	struct sockaddr_in fsin;
	int msock;
	fd_set rfds;
	fd_set afds;
	int alen;
	int fd, nfds;

	switch(argc){
		case 1:
			break;
		case 2:
			service = argv[1];
			break;
		default:
			errexit("Usage: %s [port]\n", argv[0]);
	}

	msock = passiveTCP(service, QLEN);

	nfds = getdtablesize();
	printf("getdtablesize: %d\n", nfds);
	FD_ZERO(&afds);
	FD_SET(msock, &afds);

	while (1) {
		memcpy(&rfds, &afds, sizeof(rfds));

		if (select(nfds, &rfds, (fd_set*)0, (fd_set*)0, (struct timeval*)0) < 0) {
			if (errno != EINTR)
				errexit("Select failed: %s\n", strerror(errno));
			else
				continue;
		}
		if (FD_ISSET(msock, &rfds)) {
			int ssock;
			alen = sizeof(fsin);
			ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
			if (ssock < 0)
				errexit("Accept failed: %s\n", strerror(errno));
			FD_SET(ssock, &afds);
		}
		/* Simultaneously read other slave sockets as well */
		for (fd = 0; fd < nfds ; ++fd)
			if (fd != msock && FD_ISSET(fd, &rfds))
				if (echo(fd) == 0) {
					close(fd);
					FD_CLR(fd, &afds);
				}
	}
	return 0;
}

int echo(int fd)
{
	char buf[BUFSIZE];
	int cc;

	cc = read(fd, buf, sizeof buf);
	if (cc < 0)
		errexit("echo read failed: %s\n", strerror(errno));
	if (cc && write(fd, buf, cc) < 0)
		errexit("echo write failed: %s\n", strerror(errno));
	return cc;	
}
