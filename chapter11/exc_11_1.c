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
#include <time.h>

#define QLEN	5
#define BUFSIZE	128
#define THROTTLE	1300

int errexit(const char *format, ...);
int passiveTCP(const char *service, const int qlen);
int TCPechod(int fd, int port);
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
	signal(SIGCHLD, reaper);

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
			exit(TCPechod(ssock, ntohs(fsin.sin_port)));
		default: /* parent - not using slave socket*/
			close(ssock);
			break;
		case -1:
			errexit("Fork failed: %s\n", strerror(errno));
		}
	}
	return 0;
}


int TCPechod(int fd, int port)
{
	char buf[BUFSIZE];
	int cc;
	time_t now;
	int throttling = THROTTLE;

	time(&now);
	printf("Slave[%d]: User[%d] logged in %s", getpid(), port, ctime(&now));
	while(cc = read(fd, buf, sizeof buf)) {
		if (cc < 0) {
			time(&now);
			printf("Slave[%d]: User[%d] logged off %s", getpid(), port, ctime(&now));
			errexit("Read failure: %s\n", strerror(errno));
		}
		if (write(fd, buf, cc) < 0)
				errexit("Write failure: %s\n", strerror(errno));
		if (!throttling--){
			buf[cc-1] = '\0';
			printf("Slave[%d]: User[%d], Echoed: %s, %s", getpid(), port, buf, ctime(&now));
			throttling = THROTTLE;
		}
	}
	time(&now);
	printf("Slave[%d]: User[%d] logged off %s", getpid(), port, ctime(&now));
	return 0;
}

void reaper(int sig)
{
	int status;

	while (wait3(&status, WNOHANG, (struct rusage *)0) >= 0)
			/* empty */;
}
