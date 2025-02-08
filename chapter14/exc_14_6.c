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
	char *sv_prognm;
};

int errexit(const char *format, ...);
int passiveUDP(const char* service);
int passiveTCP(const char* service, int qlen);
void doTCP(struct service *psv);
void reaper(int sig);
void cfgreader(int);
int TCPechod(int), TCPchargend(int), TCPdaytimed(int), TCPtimed(int);
void resetServerFrmCfg(struct service *fd2sv[NOFILE], int *nfds, fd_set *afds);

struct service svent[] = { /* Master service socket table/map */
	{"echo", TCP_SERV, NOSOCK, TCPechod, "inetd_echod"},
	{"chargen", TCP_SERV, NOSOCK, TCPchargend, "inetd_chargend"},
	{"daytime", TCP_SERV, NOSOCK, TCPdaytimed, "inetd_daytimed"},
	{"time", TCP_SERV, NOSOCK, TCPtimed, "inetd_timed"},
	{0, 0, 0, 0, 0}
};

int readCfg;

/* --- Super-server Multiservice server ---*/

int main (int argc, char* argv[]) 
{
	struct service *psv, *fd2sv[NOFILE];/* Master socket to service handler map */
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
	resetServerFrmCfg(fd2sv, &nfds, &afds);
	signal(SIGCHLD, reaper);
	signal(SIGHUP, cfgreader);
	while (1) {
		if (readCfg) {
			resetServerFrmCfg(fd2sv, &nfds, &afds);
			readCfg = 0;
		}
		memcpy(&rfds, &afds, sizeof(rfds));
		if (select(nfds, &rfds, (fd_set*)0, (fd_set*)0, (struct timeval*)0) < 0) {
			if (errno == EINTR) {
				if (!readCfg)
					printf("+Entering reaper...\n");
				continue;
			}
			errexit("Select error %s\n", strerror(errno));
		}
		for (fd = 0 ; fd < nfds ; ++fd) {
			if (FD_ISSET(fd, &rfds)) {
				psv = fd2sv[fd];
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
	struct sockaddr_in fsin; /* client connection */
	int alen, pid;
	int fd, ssock;
	char abspath[1024] = {0};
	char *arg[1] = {0};

	alen = sizeof(fsin);
	ssock = accept(psv->sv_sock, (struct sockaddr *)&fsin, &alen);
	if (ssock < 0)
		errexit("Accept error: %s\n", strerror(errno));
	switch(pid = fork()){
		case 0:
			printf("\tEntering child/slave process\n");
			break;
		case -1:
			errexit("Fork error: %s\n", strerror(errno));
		default: /* Master/Parent code, close slave socket */
			printf("Forked client handler successfully [%d]\n", pid);
			close(ssock);
			return ;
	}
	/* Child code, close all master sockets */
	for (fd = NOFILE; fd >=3; --fd)
		if (fd != ssock)
			close(fd);
	getcwd(abspath, 1024);
	strcat(abspath, "/");
	strcat(abspath, psv->sv_prognm);
	printf("\tLaunching service program [%s]\n", abspath);
	if (dup2(ssock, 0) < 0)
		errexit("\tdup2 failed on slave: %s\n", strerror(errno));
	if (execv(abspath, arg) < 0)
		errexit("\texecv failed to launch server: %s\n", strerror(errno));
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

void cfgreader(int)
{
	readCfg = 1;
	printf("Reconfiguring service sockets for listening\n");
	return ;
}

void resetServerFrmCfg(struct service *fd2sv[NOFILE], int *nfds, fd_set *afds)
{
	struct service *psv;
	/* Reset every open socket that is opened for a service */
	for (psv = &svent[0]; psv->sv_name; ++psv) {
		if (psv->sv_sock != NOSOCK){
			fd2sv[psv->sv_sock] = NULL;
			FD_CLR(psv->sv_sock, afds);
			close(psv->sv_sock);
		}
	}
	/* Assign sockets for service that must be opened for listening */
	for (psv = &svent[0]; psv->sv_name; ++psv) {
		if (psv->sv_useTCP)
			psv->sv_sock = passiveTCP(psv->sv_name, QLEN);
		else
			psv->sv_sock = passiveUDP(psv->sv_name);
		fd2sv[psv->sv_sock] = psv;
		*nfds = MAX(psv->sv_sock + 1, *nfds);
		FD_SET(psv->sv_sock, afds);
	}
	return;
}
