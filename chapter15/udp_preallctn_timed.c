#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/errno.h>

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int errexit(const char *format, ...);
int passiveUDP(const char *service);

#define UNIXEPOCH	2208988800 /* time since 1/1/1900 to unix epoch 1/1/1970 in sec */
#define NCRTLVL	2

int serviceHandler(int msock);
void reaper(int);

int main (int argc, char* argv[]) 
{

	int msock, pid, l;
	char *service = "time";

	switch(argc){
		case 1:
			break;
		case 2:
			service = argv[1];
			break;
		default:
			errexit("Usage: %s [port]\n", argv[0]);
	}
	msock = passiveUDP(service);
	signal(SIGCHLD, reaper);

	for (l = 0; l < NCRTLVL ; ++l){
		pid = fork(); 
		if (pid  == 0) {
			/* child  */
			printf("%d : serviceHandler starting up...\n", getpid());
			serviceHandler(msock);
			break;
		}
		else if (pid  < 0) {
			errexit("Fork failed: %s\n", strerror(errno));
		}
		else /*if (pid  > 0)*/ {
			/* parent - after forking last slave, become slave */
			if (l == NCRTLVL-1) {
				printf("%d : serviceHandler(clever master) starting up...\n", getpid());
				serviceHandler(msock);
				break;
			}
		}
	}
	printf("%d : serviceHandler shutting down\n", getpid());
	return 0;
}

int serviceHandler(int msock)
{
	struct sockaddr_in fsin = {0};
	char buf[1];
	time_t now;
	socklen_t alen;

	while (1) {
		alen = sizeof(fsin);
		if (recvfrom(msock, buf, sizeof(buf), 0, (struct sockaddr *)&fsin, &alen) < 0)
			errexit("%d: error in recvfrom: %s\n", getpid(), strerror(errno));
		
		time(&now);
		now = htonl((u_long)(now + UNIXEPOCH));
		if (sendto(msock, (char*)&now, sizeof(now), 0, (struct sockaddr *)&fsin, alen) < 0)
			errexit("%d: error in sendto: %s\n", getpid(), strerror(errno));

		fprintf(stdout, "%d: time request replies successfully\n", getpid());
	}
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
