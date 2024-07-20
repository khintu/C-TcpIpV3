#include <sys/types.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <netinet/in.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
	int status;

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
	//signal(SIGCHLD, reaper);

	while (1) {
		alen = sizeof(fsin);
		ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
		if (ssock < 0) {
			if (errno == EINTR)
				continue;
			else
				errexit("Accept failed: %s\n", strerror(errno));
		}
		switch(fork()) {
		case 0: /* child - not using master socket */
			close(msock);
			exit(TCPechod(ssock));
		default: /* parent - not using slave socket*/
			close(ssock);
			break;
		case -1:
			errexit("Fork failed: %s\n", strerror(errno));
		}
		wait3(&status, WNOHANG, (struct rusage *)0);
	}
	return 0;
}

int TCPechod(int fd)
{
	char buf[BUFSIZE];
	int cc;

	while(cc = read(fd, buf, sizeof buf)) {
		if (cc < 0)
			errexit("Read failure: %s\n", strerror(errno));
		if (write(fd, buf, cc) < 0)
				errexit("Write failure: %s\n", strerror(errno));
	}
	return 0;
}

void reaper(int sig)
{
	int status;

	while (wait3(&status, WNOHANG, (struct rusage *)0) >= 0)
			/* empty */;
}
