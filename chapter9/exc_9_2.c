#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>


int errexit(const char *format, ...);
int passiveUDP(const char *service);

#define UNIXEPOCH	2208988800 /* time since 1/1/1900 to unix epoch 1/1/1970 in sec */

int main (int argc, char* argv[]) 
{
	struct sockaddr_in fsin = {0};
	char *service = "time";
	char buf[1];
	int sock;
	time_t now;
	socklen_t alen;

	switch(argc){
		case 1:
			break;
		case 2:
			service = argv[1];
			break;
		default:
			errexit("Usage: %s [port]\n", argv[0]);
	}

	sock = passiveUDP(service);

	while (1) {
		alen = sizeof(fsin);
		if (recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr *)&fsin, &alen) < 0)
			errexit("error in recvfrom: %s\n", strerror(errno));
		time(&now);
		now = htonl((u_long)(now + UNIXEPOCH));
		// Clobber fsin
		memset(&fsin, 0x64, alen);
		if (sendto(sock, (char*)&now, sizeof(now), 0, (struct sockaddr *)&fsin, alen) < 0)
			errexit("error in sendto: %s\n", strerror(errno));
	}
	return 0;
}
