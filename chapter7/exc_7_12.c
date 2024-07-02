#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int connectUDP(const char *, const char *);
int errexit(const char *,...);
int UDPecho(const char *host, const char *service);
int sendDgram(int s, void *buf, int nchars);
int recvDgram(int s, void* buf, int nchars);

#define LINELEN	128

int main (int argc, char* argv[]) 
{
	char* host = "localhost", *service = "echo";

	switch(argc) {
		case 1:
			//use default values
			break;
		case 3:
			service = argv[2];
		case 2:
			host = argv[1];
			break;
		default:
			fprintf(stderr, "usage: %s [host [port]]\n", argv[0]);
			exit(1);
	}
	UDPecho(host, service);
	return 0;
}



int UDPecho(const char *host, const char *service)
{
	char buf[LINELEN+1];
	char rcvbuf[LINELEN+1];
	int s, n;
	int nchars;

	if ((s = connectUDP(host, service)) < 0)
		errexit("Could not connect to server\n");

	while(fgets(buf, sizeof buf, stdin)) {
		nchars = strlen(buf);
		sendDgram(s, buf, nchars);
		if (recvDgram(s, rcvbuf, nchars) < 0) {
			printf("Nothing to read, try again, msg lost\n");
			continue;
		}
		rcvbuf[nchars] = '\0';
		if (strcmp(buf, rcvbuf) != 0) {
			printf("Duplicated or lost dgram received (%s)\n", rcvbuf);
		}
		else
			fputs(rcvbuf, stdout);
	}
	close(s);
	return 0;
}
