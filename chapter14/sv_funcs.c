#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#define BUFFERSIZE	4096
#define LINELEN		72
 
int errexit(const char* format, ...);

int TCPdaytimed(int fd)
{
	char buf[LINELEN];
	time_t now;
	time(&now);
	sprintf(buf, "%s", ctime(&now));
	write(fd, buf, strlen(buf));
	return 0;
}

int TCPechod2(int fd)
{
	char buf[BUFFERSIZE];
	int cc;

	cc = read(fd, buf, sizeof buf);
	if (cc < 0)
		errexit("\techo read error: %s\n", strerror(errno));
	if (write(fd, buf, cc) < 0)
		errexit("\techo write error: %s\n", strerror(errno));
	return cc;
}

int TCPechod(int fd)
{
	char buf[BUFFERSIZE];
	int cc;

	while (cc = read(fd, buf, sizeof buf)) {
		if (cc < 0)
			errexit("\techo read error: %s\n", strerror(errno));
		if (write(fd, buf, cc) < 0)
			errexit("\techo write error: %s\n", strerror(errno));
	}
	return 0;
}

#define UNIXEPOCH	2208988800

int TCPtimed(int fd)
{
	int cc;
	time_t now;
	if ((cc = read(fd, &now, 1)) < 0)
		errexit("\ttime read error: %s\n", strerror(errno));
	else if (cc == 0) {
		printf("\tConnection closed by peer\n");
		return 0;
	}
	time(&now);
	now = htonl((u_long)now + UNIXEPOCH);
	write(fd, (char*)&now, sizeof now);
	return 0;
}

int UDPtimed(int sock)
{
	struct sockaddr_in fsin;
	int alen;
	time_t now;
	alen = sizeof fsin;
	if (recvfrom(sock, &now, sizeof now, 0, (struct sockaddr *)&fsin, &alen) < 0)
		errexit("\tudp time recvfrom error: %s\n", strerror(errno));
	time(&now);
	now = htonl((u_long)now + UNIXEPOCH);
	if (sendto(sock, &now, sizeof now, 0, (struct sockaddr *)&fsin, alen) < 0)
		errexit("\tudp time sendto error: %s\n", strerror(errno));
	return 0;
}

int TCPchargend2(int fd)
{
	int cc;
	static char c = ' ';
	char buf[LINELEN+2];
	
	if ((cc = read(fd, buf, 1)) < 0)
		errexit("\tchargen read error: %s\n", strerror(errno));
	else if (cc == 0) {
		printf("\tConnection closed by peer\n");
		return 0;
	}


	buf[LINELEN] = '\r';
	buf[LINELEN+1] = '\n';
	int i;
	for(i = 0 ; i < LINELEN; ++i) {
		buf[i] = c++;
		if (c >= '~')
			c = ' ';
	}
	if ((cc = send(fd, buf, LINELEN+2, MSG_NOSIGNAL)) < 0) {
		if (errno == EPIPE){
			printf("\tchargen write error: %s\n", strerror(errno));
			return 0; /* connection closed */
		}
		else 
			errexit("\tchargen write error: %s\n", strerror(errno));
	}
	//printf("Bytes sent %d\n", cc);
	return cc;
}

int TCPchargend(int fd)
{
	char c, buf[LINELEN+2];
	
	c = ' ';
	buf[LINELEN] = '\r';
	buf[LINELEN+1] = '\n';
	while(1) {
		int i;
		for(i = 0 ; i < LINELEN; ++i) {
			buf[i] = c++;
			if (c >= '~')
				c = ' ';
		}
		if (write(fd, buf, LINELEN+2) < 0) 
			errexit("\tchargen write error: %s\n", strerror(errno));
	}
	return 0;
}
