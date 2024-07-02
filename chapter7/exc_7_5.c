#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <arpa/inet.h>

int connectUDP(const char *, const char *);
int readUdpTimed(int s, void* buf, int buflen, time_t timeout_in_sec);
int errexit(const char *,...);
int readFromTimeSrv(char *host, char* service, time_t *now);

#define BUFSIZE	64
#define UNIXEPOCH		2208988800
#define MSG					"what time is it?\n"

int main (int argc, char* argv[]) 
{
	char* host1, *host2, *service = "time";
	time_t now1, now2, diff;

	switch(argc) {
		case 3:
			host2 = argv[2];
			host1 = argv[1];
			break;
		default:
			fprintf(stderr, "usage: %s host1 host2\n", argv[0]);
			exit(1);
	}
	readFromTimeSrv(host1, service, &now1);
	readFromTimeSrv(host2, service, &now2);
	printf("%g\n", difftime(now2, now1));
	return 0;
}

int readFromTimeSrv(char *host, char* service, time_t *now)
{
	int s, n;

	s = connectUDP(host, service);

	write(s, MSG, strlen(MSG));

	n = readUdpTimed(s, (char*)now, sizeof *now, 5);
	if (n < 0)
		errexit("Read failed: %s\n", strerror(errno));
	*now = ntohl((u_long)*now);
	*now -= UNIXEPOCH;
	printf("%s", ctime(now));
	close(s);
	return 0;
}
