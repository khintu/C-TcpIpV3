#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

int connectUDP(const char *, const char *);
int errexit(const char *,...);
int UDPecho(const char *host, const char *service);
int readUdpTimed2(int s, void* buf, int buflen, time_t timeout_in_sec);

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
	int s, n, retry;
	int nchars;
	time_t t1, t2;

	if ((s = connectUDP(host, service)) < 0)
		errexit("Could not connect to server\n");

	while(fgets(buf, sizeof buf, stdin)) {
		nchars = strlen(buf);
		retry = 2;
		while (retry--) {
			time(&t1);
			if (write(s, buf, nchars) < 0)
				errexit("Write error: %s\n", strerror(errno));
			
			if ((n = readUdpTimed2(s, buf, nchars, 5)) < 0) {
				if (n == -1)
					errexit("Reply receive error...exiting\n");
				else {
					if (!retry){
						errexit("Host unreachable\n");
					}
					else {
						printf("Retrying sending...\n");
						continue;
					}
				}
			}
			time(&t2);
			printf("Reply received in %g sec\n", difftime(t2, t1));
			fputs(buf, stdout);
			retry = 0;
		}
	}
	close(s);
	return 0;
}
