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

#define QLEN	2
#define BUFSIZE	4096
#define NCRTLVL	2

int errexit(const char *format, ...);
int passiveTCP(const char *service, const int qlen);
int serviceHandler(int msock);
int TCPechod(int fd);
void reaper(int);

int main (int argc, char* argv[]) 
{
	char *service = "echo";
	int err;
	int msock;

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

	for (int l = 0; l < NCRTLVL ; ++l){
		err = fork(); 
		if (err == 0) {
			/* child  */
			printf("%d : serviceHandler starting up...\n", getpid());
			serviceHandler(msock);
			break;
		}
		else if (err < 0) {
			errexit("Fork failed: %s\n", strerror(errno));
		}
		else /*if (err > 0)*/ {
			/* parent - after forking last slave, become slave */
			if (l == NCRTLVL-1) {
				printf("%d : serviceHandler(clever root) starting up...\n", getpid());
				serviceHandler(msock);
				break;
			}
		}
	}
	printf("%d : servicerHandler shutting down\n", getpid());
	return 0;
}

int serviceHandler(int msock)
{
	struct sockaddr_in fsin;
	int ssock;
	int alen;

	while (1) {
		alen = sizeof(fsin);
		ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
		if (ssock < 0) {
			if (errno == EINTR){
				printf("Do you want to exit(Y/N): ");
				if (getchar() == 'Y')
					errexit("Shutting down echo server\n");
				else
					continue;
			}
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

	printf("%d : New connection accepted %d\n", getpid(), fd);
	while(cc = read(fd, buf, sizeof buf)) {
		if (cc < 0)
			errexit("Read failure: %s\n", strerror(errno));
		if (write(fd, buf, cc) < 0)
				errexit("Write failure: %s\n", strerror(errno));
	}
	printf("%d : connection closed %d\n", getpid(), fd);
	return 0;
}

void reaper(int sig)
{
	int status, pid;
	while((pid = wait3(&status, WNOHANG, (struct rusage*)0)) > 0) {
		printf("-+Z+-Reaper read status %d, pid %d\n", WEXITSTATUS(status), pid);
	}
	return;
}
