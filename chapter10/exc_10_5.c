#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>


int errexit(const char *format, ...);
int passiveTCP(const char *service, const int qlen);
int TCPdaytimed(int fd);

#define QLEN	5

int main (int argc, char* argv[]) 
{
	struct sockaddr_in fsin;
	char *service = "daytime";
	int msock, ssock;
	int alen;

	switch(argc) {
		case 1:
			break;
		case 2:
			service = argv[1];
			break;
		default:
			errexit("Usage: %s [port]\n", argv[0]);
	}
	msock = passiveTCP(service, QLEN);

	while (1) {
		ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
		if (ssock < 0)
			errexit("Accept failed: %s\n", strerror(errno));
		TCPdaytimed(ssock);
		//close(ssock);
	}
	return 0;
}

int TCPdaytimed(int fd)
{
	char *pts;
	time_t now;
	
	time(&now);
	pts = ctime(&now);
	write(fd, pts, strlen(pts));
	return 0;
}
