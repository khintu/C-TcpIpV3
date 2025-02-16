#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>

#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

#define MAX(x, y)	((x) > (y))?(x):(y)
#define LINELEN	128


int dodaytime(char buf[], int *buflen);
int doecho(char buf[], int *buflen);
int dotime(char buf[], int *buflen);
int errexit(const char *format, ...);
int passiveUDP(const char* service);
void reaper(int);
int serviceHandler(int);

extern u_short portbase;	/* for non-root servers */

#define MAXSVCMAP	3
#define NCRTLVL	2

struct service {
	int sockfd;
	int (*handler)(char buf[], int *size);
} svcmap[MAXSVCMAP];

/* --- Concurrent UDP Multiservice server ---*/

int main (int argc, char* argv[]) 
{
	int usock1, usock2, usock3; 
	int nfds, pid, l;
	switch(argc){
		case 1:
			break;
		case 2:
			portbase = atoi(argv[1]);
			break;
		default:
			errexit("Usage: multisvcd [postbase]\n");
	}
	signal(SIGCHLD, reaper);
	usock1 /* echo */ = passiveUDP("echo");
	usock2 /* daytime */ = passiveUDP("daytime");
	usock3 /* time */ = passiveUDP("time");
	svcmap[0].sockfd = usock1; svcmap[0].handler = doecho;
	svcmap[1].sockfd = usock2; svcmap[1].handler = dodaytime;
	svcmap[2].sockfd = usock3; svcmap[2].handler = dotime;
	nfds = 0;
	nfds = MAX(nfds, usock1);
	nfds = MAX(nfds, usock2);
	nfds = MAX(nfds, usock3);
	
	for (l = 0; l < NCRTLVL ; ++l){
		pid = fork(); 
		if (pid  == 0) {
			/* child  */
			printf("%d : serviceHandler starting up...\n", getpid());
			serviceHandler(nfds);
			break;
		}
		else if (pid  < 0) {
			errexit("Fork failed: %s\n", strerror(errno));
		}
		else /*if (pid  > 0)*/ {
			/* parent - after forking last slave, become slave */
			if (l == NCRTLVL-1) {
				printf("%d : serviceHandler(clever master) starting up...\n", getpid());
				serviceHandler(nfds);
				break;
			}
		}
	}
	printf("%d : serviceHandler shutting down\n", getpid());

	return 0;
}

int serviceHandler(int nfds)
{
	struct sockaddr_in fsin;
	int alen, sock, idx, buflen;
	fd_set rfds;

	FD_ZERO(&rfds);
	while (1) {
		FD_SET(svcmap[0].sockfd, &rfds);	
		FD_SET(svcmap[1].sockfd, &rfds);	
		FD_SET(svcmap[2].sockfd, &rfds);	
		if (select(nfds+1, &rfds, (fd_set*)0, (fd_set*)0, (struct timeval*)0) < 0)
			errexit("Select error %s\n", strerror(errno));
	
		for (idx = 0; idx < MAXSVCMAP; ++idx) {
			sock = svcmap[idx].sockfd;
			if (FD_ISSET(sock, &rfds)){
				char buf[LINELEN+1] = {0};
				alen = sizeof fsin;
				if ((buflen = recvfrom(sock, buf, sizeof buf, 0, (struct sockaddr *)&fsin, &alen)) < 0)
					errexit("recvfrom error: %s\n", strerror(errno));
				printf("%d : Datagram received for socket %d\n", getpid(), sock);
				svcmap[idx].handler(buf, &buflen);
				if (sendto(sock, buf, buflen, 0, (struct sockaddr *)&fsin, alen) < 0)
					errexit("sendto error: %s\n", strerror(errno));
				printf("%d : Datagram reply sent for socket %d\n", getpid(), sock);
			}
		}
	}

	return 0;
}

int dodaytime(char buf[], int *buflen)
{
	time_t now;
	time(&now);
	sprintf(buf, "%s", ctime(&now));
	*buflen = strlen(buf);
	return 0;
}

int doecho(char buf[], int *buflen)
{
	*buflen = strlen(buf);
	return 0;
}

#define UNIXEPOCH	2208988800

int dotime(char buf[], int *buflen)
{
	time_t now;
	time(&now);
	now = htonl((u_long)now + UNIXEPOCH);
	memcpy(buf, (void*)&now, sizeof now);
	*buflen = sizeof now;
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
