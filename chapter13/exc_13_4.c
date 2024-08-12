#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#define FALSE	0
#define TRUE	1

int daytime(char buf[]);
int errexit(const char *format, ...);
int passiveTCP(const char *service, const int qlen);
int passiveUDP(const char *service);

#define MAX(x, y)	(((x) > (y)) ? (x) : (y))
#define QLEN	5
#define LINELEN	128

int main (int argc, char* argv[]) 
{
	char buf[LINELEN+1], *str;
	struct sockaddr_in fsin;
	int alen;
	int tsock;
	int usock;
	int nfds;
	fd_set rfds;
	char* tcpport, *udpport;
	int isTcp = FALSE, isUdp = FALSE;
	
	while (--argc > 0 && (str = *++argv)){
		if (*str++ == '-'){
			if(strncmp(str, "TCP", 3) == 0){
				tcpport = str+4; isTcp = TRUE;
			}
			else if(strncmp(str, "UDP", 3) == 0){
				udpport = str+4; isUdp = TRUE;
			}
			else
				break;
		}
		else
			break;
	}
	if (argc != 0)
			errexit("Usage: daytimed [-TCP=port] [-UDP=port]\n");
	
	if (isTcp && isUdp){
		tsock = passiveTCP(tcpport, QLEN);
		usock = passiveUDP(udpport);
		nfds = MAX(tsock, usock) + 1;
	} else if (isTcp){
		tsock = passiveTCP(tcpport, QLEN);
		nfds = tsock + 1;
	}
	else {
		usock = passiveUDP(udpport);
		nfds = usock + 1;
	}

	FD_ZERO(&rfds);
	
	while (1) {
		if (isTcp && isUdp) {
			FD_SET(tsock, &rfds);
			FD_SET(usock, &rfds);
		}
		else if (isTcp)
			FD_SET(tsock, &rfds);
		else
			FD_SET(usock, &rfds);

		if (select(nfds, &rfds, (fd_set*)0, (fd_set*)0, (struct timeval*)0) < 0)
			errexit("Select error: %s\n", strerror(errno));
		if (isTcp && FD_ISSET(tsock, &rfds)) {
			int ssock;
			alen = sizeof fsin;
			ssock = accept(tsock, (struct sockaddr *)&fsin, &alen);
			if (ssock < 0)
				errexit("Accept error: %s\n", strerror(errno));
			daytime(buf);
			write(ssock, buf, strlen(buf));
			close(ssock);
		}
		if (isUdp && FD_ISSET(usock, &rfds)) {
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
	return 0;
}
