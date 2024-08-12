#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>

void reaper(int);
int daytime(char buf[]);
int errexit(const char *format, ...);
int passiveTCP(const char *service, const int qlen);
int passiveUDP(const char *service);

#define MAX(x, y)	(((x) > (y)) ? (x) : (y))
#define QLEN	5
#define LINELEN	128

int main (int argc, char* argv[]) 
{
	char *service = "daytime";
	char buf[LINELEN+1];
	struct sockaddr_in fsin;
	int cc;
	int alen;
	int tsock;
	int usock;
	int nfds;
	fd_set rfds;

	switch(argc){
		case 1:
			break;
		case 2:
			service = argv[1];
			break;
		default:
			errexit("Usage: %s [port]\n", argv[0]);
	}
	
	tsock = passiveTCP(service, QLEN);
	usock = passiveUDP(service);
	nfds = MAX(tsock, usock) + 1;

	FD_ZERO(&rfds);

	signal(SIGCHLD, reaper);
		
	while (1) {
		FD_SET(tsock, &rfds);
		FD_SET(usock, &rfds);

		if ((cc = select(nfds, &rfds, (fd_set*)0, (fd_set*)0, (struct timeval*)0)) < 0) {
			if (errno != EINTR)
				errexit("Select error: %s\n", strerror(errno));
			printf("Entering reaper...\n");
			//break;
			continue;
		}
		printf("\nMaster waking up from select: %d, tsock(%d)\n", cc, FD_ISSET(tsock, &rfds));
		if (FD_ISSET(tsock, &rfds)) {
			int ssock;
			alen = sizeof fsin;
			ssock = accept(tsock, (struct sockaddr *)&fsin, &alen);
			if (ssock < 0)
				errexit("\tAccept error: %s\n", strerror(errno));
			printf("\tClient accepted, initiating slave reply handler\n");
			if ((cc = fork()) == 0) {
				close(tsock);
				daytime(buf);
				write(ssock, buf, strlen(buf));
				printf("\tReplied successfully, closing slave [pid=%d]\n", getpid());
				close(ssock);
				exit(2);
			}
			else if(cc == -1) {
				errexit("Could not fork slave: %s\n", strerror(errno));
			}
			else {
				close(ssock);
				printf("Forked slave successfully [pid=%d]\n", cc);
			}
		}
		if (FD_ISSET(usock, &rfds)) {
			alen = sizeof fsin;
			if (recvfrom(usock, buf, sizeof buf, 0, (struct sockaddr *)&fsin, &alen) < 0)
				errexit("Recvfrom error: %s\n", strerror(errno));
			daytime(buf);
			sendto(usock, buf, strlen(buf), 0, (struct sockaddr *)&fsin, alen);
		}
	}

	return 0;
}

int daytime(char buf[])
{
	time_t now;
	time(&now);
	sprintf(buf, "%s", ctime(&now));
}


void reaper(int)
{
	int status, pid;
	while((pid = wait3(&status, WNOHANG, (struct rusage*)0)) > 0) {
		printf("-+Z+-Reaper read status %d, pid %d\n", WEXITSTATUS(status), pid);
	}
	return ;
}
