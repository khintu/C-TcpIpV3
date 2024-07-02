#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int connectTCP(const char *, const char *);
int errexit(const char *,...);
int TCPdaytime(const char *, const char *);

#define LINELEN	128

int main (int argc, char* argv[]) 
{
	char* host = "localhost", *service = "daytime";

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
	TCPdaytime(host, service);
	return 0;
}



int TCPdaytime(const char *host, const char *service)
{
	char buf[LINELEN+1];
	int s, n, nplus = 0;
	//s = connectTCP("mischief.hello.com", "telnet");
	//s = connectTCP("localhost", "telnet");
	if ((s = connectTCP(host, service)) < 0)
		errexit("Could not connect to server\n");

	printf("Connection established...\n");

	while ((n = read(s, buf, LINELEN)) > 0){
		buf[n] = '\0';
		fputs(buf, stdout);
		nplus += n;
	}
	printf("%d bytes read\n", nplus);
	sleep(67); /* more than 2*MSL */
	close(s);

	return 0;
}
