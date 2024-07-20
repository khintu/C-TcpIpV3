#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define QLEN	5
#define BUFSIZE	4096

int errexit(const char *format, ...);
int passiveTCP(const char *service, const int qlen);
int TCPechod(int fd);
void reaper(int);

int main (int argc, char* argv[]) 
{
	struct sockaddr_in fsin;
	char *service = "echo";
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
		alen = sizeof(fsin);
		ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
		if (ssock < 0) {
			if (errno == EINTR)
				continue;
			else
				errexit("Accept failed: %s\n", strerror(errno));
		}
		TCPechod(ssock);
		close(ssock);
	}
	return 0;
}

int TCPechod(int fd)
{
	char buf[BUFSIZE];
	int cc;

	while(cc = read(fd, buf, sizeof buf)) {
		if (cc < 0) /* RST on pipe caught, client crashed */
			errexit("Read failure: %s\n", strerror(errno));
		if (write(fd, buf, cc) < 0)
				errexit("Write failure: %s\n", strerror(errno));
	}
	return 0;
}
