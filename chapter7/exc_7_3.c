#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <arpa/inet.h>

int connectUDP(const char *, const char *);
int errexit(const char *,...);

#define BUFSIZE	64
#define UNIXEPOCH		2208988800
#define MSG					"what time is it?\n"

int main (int argc, char* argv[]) 
{
	char* host = "localhost", *service = "time";
	time_t now, t1;
	struct tm t2 = {0};
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

	s = connectUDP(host, service);

	write(s, MSG, strlen(MSG));

	n = read(s, (char*)&now, sizeof now);
	if (n < 0)
		errexit("Read failed: %s\n", strerror(errno));
	now = ntohl((u_long)now);
	now -= UNIXEPOCH;

	t2.tm_mday = 1;
	t2.tm_mon = 0;
	t2.tm_year = 125;
	t1 = mktime(&t2);
	
	if (difftime(now, t1) > 0.0f)
		printf("%s", ctime(&now));
	else
		printf("time is less than %s\n", asctime(&t2));

	close(s);
	return 0;
}
