#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/signal.h>
#include <sys/param.h>
#include <netinet/in.h>

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#ifndef MAX
#define MAX(x, y)	((x) > (y))?(x):(y)
#endif
#define LINELEN	128
#define QLEN	5

extern u_short portbase;	/* for non-root servers */

#define UDP_SERV	0
#define TCP_SERV	1
#define NOSOCK	-1	/* an invalid socket descriptor */

struct service {
	char *sv_name;
	char sv_useTCP;
	int sv_sock;
	int (*sv_func)(int);
};

int errexit(const char *format, ...);
int passiveUDP(const char* service);
int passiveTCP(const char* service, int qlen);
void doTCP(struct service *psv);
void reaper(int sig);
int TCPechod(int), TCPchargend(int), TCPdaytimed(int), TCPtimed(int);

struct service svent[] = { /* Master service socket table/map */
	{"echo", TCP_SERV, NOSOCK, TCPechod},
	{"chargen", TCP_SERV, NOSOCK, TCPchargend},
	{"daytime", TCP_SERV, NOSOCK, TCPdaytimed},
	{"time", TCP_SERV, NOSOCK, TCPtimed},
	{0, 0, 0, 0}
};

/* --- Super-server Multiservice server ---*/

int main (int argc, char* argv[]) 
{
	struct service *psv;/* Master socket to service handler map */
	int nfds, fd;
	fd_set afds, rfds;

	switch(argc){
		case 1:
			break;
		case 2:
			portbase = atoi(argv[1]);
			break;
		default:
			errexit("Usage: superd [postbase]\n");
	}
	
	nfds = 0;
	FD_ZERO(&afds);
	/* Configure Master sockets and assign descriptor set */
	for (psv = &svent[0]; psv->sv_name; ++psv) {
		if (psv->sv_useTCP)
			psv->sv_sock = passiveTCP(psv->sv_name, QLEN);
		else
			psv->sv_sock = passiveUDP(psv->sv_name);
		nfds = MAX(psv->sv_sock + 1, nfds);
		if (nfds > FD_SETSIZE)
			errexit("Select max fd number exceeded limit [%d]\n", FD_SETSIZE);
		FD_SET(psv->sv_sock, &afds);
	}
	signal(SIGCHLD, reaper);
	while (1) {
		memcpy(&rfds, &afds, sizeof(rfds));
		if (select(nfds, &rfds, (fd_set*)0, (fd_set*)0, (struct timeval*)0) < 0) {
			if (errno == EINTR) {
				printf("+Entering reaper...\n");
				continue;
			}
			errexit("Select error %s\n", strerror(errno));
		}
		for (fd = 0 ; fd < nfds ; ++fd) {
			if (FD_ISSET(fd, &rfds)) {
				for (psv = &svent[0]; psv->sv_name; ++psv)
					if (psv->sv_sock == fd)
						break;
				if (psv->sv_name == NULL)
					errexit("Service map entry for fd set not found...aborting\n");
				if (psv->sv_useTCP)
					doTCP(psv); /* Create slave socket, handle a TCP connection */
				else
					psv->sv_func(psv->sv_sock);
			}
		}
	}
	return 0;
}

/* Handle a TCP service client connection (slave socket) */
void doTCP(struct service *psv)
{
	struct service *psv1;
	struct sockaddr_in fsin; /* client connection */
	int alen, pid;
	int fd, ssock;

	alen = sizeof(fsin);
	ssock = accept(psv->sv_sock, (struct sockaddr *)&fsin, &alen);
	if (ssock < 0)
		errexit("Accept error: %s\n", strerror(errno));
	switch(pid = fork()){
		case 0:
			break;
		case -1:
			errexit("Fork error: %s\n", strerror(errno));
		default: /* Master/Parent code, close slave socket */
			printf("Forked client handler successfully [%d], fd[%d]\n", pid, ssock);
			close(ssock);
			return ;
	}
	/* Child code, close all master sockets */
	for (psv1 = &svent[0]; psv1->sv_name; ++psv1)
		close(psv1->sv_sock);
	exit(psv->sv_func(ssock));
	return ;
}

void reaper(int)
{
	int status, pid;
	while((pid = wait3(&status, WNOHANG, (struct rusage*)0)) > 0) {
		printf("-+Z+-Reaper read status %d, pid %d\n", WEXITSTATUS(status), pid);
	}
	return ;
}
