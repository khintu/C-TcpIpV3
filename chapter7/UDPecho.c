#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

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
	char buf[LINELEN+1];
	int s;
	int nchars;

	if ((s = connectUDP(host, service)) < 0)
		errexit("Could not connect to server\n");

	while(fgets(buf, sizeof buf, stdin)) {
		nchars = strlen(buf);
		if (write(s, buf, nchars) < 0)
			errexit("Write error: %s\n", strerror(errno));
		
		if (read(s, buf, nchars) < 0)
			errexit("Read error: %s\n", strerror(errno));
		fputs(buf, stdout);
	}
	close(s);
	return 0;
}
