#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

int connectUDP(const char *, const char *);
int errexit(const char *,...);
int UDPecho(const char *host, const char *service);

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
	char *msg = "Testing echo";
	char buf[LINELEN + 1];
	int s, nchars;
	time_t t1, t2;

	if ((s = connectUDP(host, service)) < 0)
		errexit("Could not connect to server\n");

	while (1) {
		nchars = strlen(msg);
		strcpy(buf, msg);
		time(&t1);
		if (write(s, buf, nchars) < 0)
			errexit("Write error: %s\n", strerror(errno));
		
		if (read(s, buf, nchars) < 0)
			errexit("Read error: %s\n", strerror(errno));
		time(&t2);
		buf[nchars] = '\0';
		if (strcmp(msg, buf) != 0)
			errexit("Echo reply incorrect %s\n", buf);
		printf("Reply in %g\n", difftime(t2, t1));
		usleep(1000000);
	}
	close(s);
	return 0;
}
