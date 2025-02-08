#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <netinet/in.h>

#include <unistd.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define QLEN	2
#define BUFSIZE	4096

int errexit(const char *format, ...);
int passiveTCP(const char *service, const int qlen);
int serviceHandler(int msock);
int TCPechod(int fd);
void reaper(int);
void delayed_alloctn_cb(int);

jmp_buf env;
int isChild;

int main (int argc, char* argv[]) 
{
	char *service = "echo";
	int msock;
	struct sigaction sa = {0};

	switch(argc) {
		case 1:
			break;
		case 2:
			service = argv[1];
			break;
		default:
			errexit("Usage: %s [port]\n", argv[0]);
	}

	signal(SIGCHLD, reaper);
	sa.sa_handler = delayed_alloctn_cb;
	sigfillset(&sa.sa_mask);
	sa.sa_flags = SA_NODEFER;
	sigaction(SIGALRM, &sa, NULL);

	msock = passiveTCP(service, QLEN);
	serviceHandler(msock);
	close(msock);
	return 0;
}

void delayed_alloctn_cb(int)
{
	int err;
	err = fork(); 
	if (err == 0) {
		/* child  */
		printf("%d : slave serviceHandler starting up...\n", getpid());
		isChild = 1;
	}
	else if (err < 0) {
		errexit("Fork failed: %s\n", strerror(errno));
	}
	else
		/* parent - after forking slave, dont resume on client */
		siglongjmp(env,1);
	return;
}

int serviceHandler(int msock)
{
	struct sockaddr_in fsin;
	int ssock = 0;
	int alen;
	fd_set rfds;
	fd_set afds;
	int fd, nfds;
	struct timeval childtm = {0};

	nfds = getdtablesize();
	FD_ZERO(&afds);
	FD_SET(msock, &afds);


	while (1) {
		if (1 == sigsetjmp(env, 1) && ssock != 0) {
			printf("%d : Master closing client connection on timeout\n", getpid());
			close(ssock);
		}

		memcpy(&rfds, &afds, sizeof rfds);
		childtm.tv_sec = 10;
	
		if (isChild) {
			if ((fd = select(nfds, &rfds, (fd_set*)0, (fd_set*)0, &childtm)) < 0)
				errexit("Select failed: %s\n", strerror(errno));
			else if (fd == 0) {/* child inactivity timeout expired */
				printf("%d : slave servicerHandler shutting down\n", getpid());
				break;
			}
		}

		alen = sizeof(fsin);
		ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
		if (ssock < 0) {
			if (errno == EINTR){
				printf("%d : Interrupt caught in accept\n", getpid());
				continue;
			}
			else
				errexit("Accept failed: %s\n", strerror(errno));
		}
		/* Delayed Alloc: User protocol logic timing code */
		if (!isChild) { /* Master times a request */
			alarm(5);
		}
		TCPechod(ssock);
		close(ssock);
		ssock = 0;
		if (!isChild) { /* Master cancels timer on request completion */
			alarm(0);
		}
	}
	return 0;
}

int TCPechod(int fd)
{
	char buf[BUFSIZE];
	int cc;

	printf("%d : New connection accepted %d\n", getpid(), fd);
	while(cc = read(fd, buf, sizeof buf)) {
		if (cc < 0 && errno == EINTR){
			printf("%d : Interrupt caught in read\n", getpid());
			continue;
		}
		else if (cc < 0)
			errexit("Read failure: %s\n", strerror(errno));
		else{
			if (write(fd, buf, cc) < 0)
				errexit("Write failure: %s\n", strerror(errno));
		}
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
