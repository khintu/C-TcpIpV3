#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

#define QLEN	5
#define BUFSIZE	4096


int errexit(const char *format, ...);
int passiveTCP(const char *service, const int qlen);
int daytimed(int fd);

/* Concurrent TCP server for Daytime service */

int main (int argc, char* argv[]) 
{
	char *service = "echo";
	struct sockaddr_in fsin;
	int msock;
	fd_set rfds;
	fd_set wfds;
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
	//FD_SET(msock, &afds);

	while (1) {
		FD_ZERO(&rfds);
		FD_SET(msock, &rfds);
		memcpy(&wfds, &afds, sizeof(rfds));

		if (select(nfds, &rfds, &wfds, (fd_set*)0, (struct timeval*)0) < 0) {
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
			printf("Connection accepted %d\n", ntohs(fsin.sin_port));
			/*if (daytimed(ssock) == 0) {
				close(ssock);
			}*/
			FD_SET(ssock, &afds);
		}
		/* Simultaneously read other slave sockets as well */
		for (fd = 0; fd < nfds ; ++fd)
			if (fd != msock && FD_ISSET(fd, &wfds))
				if (daytimed(fd) == 0) {
					alen = sizeof(fsin);
					getpeername(fd, (struct sockaddr *)&fsin, &alen);
					printf("Closing sock %d\n", ntohs(fsin.sin_port));
					close(fd);
					FD_CLR(fd, &afds);
				}
	}
	return 0;
}

int daytimed(int fd)
{
	char *pts;
	time_t now;
	
	time(&now);
	pts = ctime(&now);
	write(fd, pts, strlen(pts));
	return 0;
}
