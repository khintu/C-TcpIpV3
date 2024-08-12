#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <arpa/inet.h>

int connectUDP(const char *, const char *);
int errexit(const char *,...);
int UDPdaytime(const char *host, const char *service);

#define LINELEN	128

int main (int argc, char* argv[]) 
{
	char* host = "localhost", *service = "daytime";
	int s, n;

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

	UDPdaytime(host, service);
	return 0;
}

int UDPdaytime(const char *host, const char *service)
{
	char buf[LINELEN+1] = "Hello";
	int s, n, nplus = 0;
	if ((s = connectUDP(host, service)) < 0)
		errexit("Could not connect to server\n");

	printf("Connection established...\n");
	write(s, buf, strlen(buf));
	while (((n = read(s, buf, LINELEN)) > 0) ||\
				(n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))){
		if (n > 0){
			buf[n] = '\0';
			fputs(buf, stdout);
			nplus += n;
			break;
		}
		else
			usleep(15000);
	}
	printf("%d bytes read\n", nplus);
	close(s);

	return 0;
}
