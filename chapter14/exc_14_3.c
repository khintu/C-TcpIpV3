#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
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
int TCPechod2(int), TCPchargend2(int), TCPdaytimed(int), TCPtimed(int);

struct service svent[] = { /* Master service socket table/map */
	{"echo", TCP_SERV, NOSOCK, TCPechod2},
	{"chargen", TCP_SERV, NOSOCK, TCPchargend2},
	{"daytime", TCP_SERV, NOSOCK, TCPdaytimed},
	{"time", TCP_SERV, NOSOCK, TCPtimed},
	{0, 0, 0, 0}
};

/* --- Super-server Multiservice server ---*/

int main (int argc, char* argv[]) 
{
	struct service *psv, *fd2svc[FD_SETSIZE] = {0};/* Master socket to service handler map */
	int nfds, fd;
	int alen;
	int ssock;
	struct sockaddr_in fsin; /* client connection */
	int isMasterSock;
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
			errexit("1.Select max fd [%d] number exceeded limit [%d]\n", nfds, FD_SETSIZE);
		FD_SET(psv->sv_sock, &afds);
	}
	while (1) {
		memcpy(&rfds, &afds, sizeof(rfds));
		if (select(nfds, &rfds, (fd_set*)0, (fd_set*)0, (struct timeval*)0) < 0)
			errexit("Select error %s\n", strerror(errno));
		for (fd = 0 ; fd < nfds ; ++fd) {
			isMasterSock = 0;
			if (FD_ISSET(fd, &rfds)) {
				/* Master sockets handler */
				for (psv = &svent[0]; psv->sv_name; ++psv) {
					if (psv->sv_sock == fd) {
						if (psv->sv_useTCP) {
							/* Create slave (new) socket, handle a TCP connection */
							alen = sizeof(fsin);
							ssock = accept(psv->sv_sock, (struct sockaddr *)&fsin, &alen);
							if (ssock < 0)
								errexit("Accept error: %s\n", strerror(errno));
							FD_SET(ssock, &afds);
							nfds = MAX(ssock + 1, nfds);
							if (nfds > FD_SETSIZE)
								errexit("2.Select max fd [%d] number exceeded limit [%d]\n", nfds, FD_SETSIZE);
							fd2svc[ssock] = psv;
						}
						else
							psv->sv_func(psv->sv_sock);
						if (strcmp(psv->sv_name, "daytime") == 0){
							if (psv->sv_func(ssock) == 0){
								close(ssock);
								fd2svc[ssock] = 0;
								FD_CLR(ssock, &afds);
							}
						}
						isMasterSock = 1;
						break;
					}
				}
				/* Existing client connections service */
				if (!isMasterSock) {
					psv = fd2svc[fd];
					if (psv && psv->sv_func(fd) == 0){
						printf("Closing connection to peer [%d]\n", fd);
						close(fd);
						fd2svc[fd] = 0;
						FD_CLR(fd, &afds);
					}
				}
			}
		}
	}
	return 0;
}
