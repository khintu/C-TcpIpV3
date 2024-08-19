#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <arpa/inet.h>

int connectTCP(const char *, const char *);
int errexit(const char *,...);

#define BUFSIZE	64
#define UNIXEPOCH		2208988800
#define MSG					"what time is it?\n"

int main (int argc, char* argv[]) 
{
	char* host = "localhost", *service = "time";
	time_t now;
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

	s = connectTCP(host, service);

	//write(s, MSG, strlen(MSG));
	write(s, MSG, 1);

	while (n = read(s, (char*)&now, sizeof now))
		if (n < 0) {
			if (errno == EWOULDBLOCK || errno == EAGAIN)
				continue;
			errexit("Read failed: %s\n", strerror(errno));
		}
		else
			break;
	now = ntohl((u_long)now);
	now -= UNIXEPOCH;
	printf("%s", ctime(&now));
	close(s);
	return 0;
}
