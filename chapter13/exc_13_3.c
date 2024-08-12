#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>

int daytime(char buf[]);
int errexit(const char *format, ...);
int passiveTCP(const char *service, const int qlen);
int passiveUDP(const char *service);

#define MAX(x, y)	(((x) > (y)) ? (x) : (y))
#define QLEN	5
#define LINELEN	128

int main (int argc, char* argv[]) 
{
	//char *service = "daytime";
	char buf[LINELEN+1], *str;
	struct sockaddr_in fsin;
	int alen;
	int tsock;
	int usock;
	int nfds;
	fd_set rfds;
	char* tcpport, *udpport;
	
	while (--argc > 0 && (str = *++argv)){
		if (*str++ == '-'){
			if(strncmp(str, "TCP", 3) == 0)
				tcpport = str+4;
			else if(strncmp(str, "UDP", 3) == 0)
				udpport = str+4;
			else
				break;
		}
		else
			break;
	}
	if (argc != 0)
			errexit("Usage: daytimed [-TCP=port] [-UDP=port]\n");
	
	tsock = passiveTCP(tcpport, QLEN);
	usock = passiveUDP(udpport);
	nfds = MAX(tsock, usock) + 1;

	FD_ZERO(&rfds);
	
	while (1) {
		FD_SET(tsock, &rfds);
		FD_SET(usock, &rfds);

		if (select(nfds, &rfds, (fd_set*)0, (fd_set*)0, (struct timeval*)0) < 0)
			errexit("Select error: %s\n", strerror(errno));
		if (FD_ISSET(tsock, &rfds)) {
			int ssock;
			alen = sizeof fsin;
			ssock = accept(tsock, (struct sockaddr *)&fsin, &alen);
			if (ssock < 0)
				errexit("Accept error: %s\n", strerror(errno));
			daytime(buf);
			write(ssock, buf, strlen(buf));
			close(ssock);
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
