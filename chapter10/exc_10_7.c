#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#define QLEN	5
#define TABLE_LEN	100

struct SlotItem {
	short isActive;
	int port; /* unique index into table */
	time_t closeTm;
};

struct SlotItem ConnTable[TABLE_LEN];

int errexit(const char *format, ...);
int passiveTCP(const char *service, const int qlen);
int TCPdaytimed(int fd);
int preprocessingRequest(int);
void postprocessingReply(int);


int main (int argc, char* argv[]) 
{
	struct sockaddr_in fsin;
	char *service = "daytime";
	int msock, ssock;
	int alen;

	switch(argc) {
		case 1:
			break;
		case 2:
			service = argv[1];
			break;
		default:
			errexit("Usage: %s [port]\n", argv[0]);
	}
	printf("Iterative/Stateful daytime server\n");
	msock = passiveTCP(service, QLEN);

	while (1) {
		ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
		if (ssock < 0)
			errexit("Accept failed: %s\n", strerror(errno));
		if (preprocessingRequest(ssock) < 0){
			printf("Cannot service anymore connections!! Try after 1 min\n");
			close(ssock);
			continue;
		}
		TCPdaytimed(ssock);
		postprocessingReply(ssock);
	}
	return 0;
}

int preprocessingRequest(int fd)
{
	int i;
	time_t now;
	struct sockaddr_in fsin;
	socklen_t alen;
	getpeername(fd, (struct sockaddr *)&fsin, &alen);


	for (i = 0 ; i < TABLE_LEN ; ++i) {
		if (ConnTable[i].isActive) {
			time(&now);
			if(difftime(now, ConnTable[i].closeTm) > 240 /* 4min*/) {
				printf("Removing entry from table[%d] %s", i, ctime(&ConnTable[i].closeTm));
				memset(&ConnTable[i], 0x0, sizeof ConnTable[TABLE_LEN]);
			}
		}
	}
	for (i = 0 ; i < TABLE_LEN ; ++i){
		if (!ConnTable[i].isActive) {
			ConnTable[i].port = ntohs(fsin.sin_port);
			ConnTable[i].isActive = 1;
			printf("Saved socket[%d] in table[%d]\n", ntohs(fsin.sin_port), i);
			return 0;
		}
	}
	return -1;
}

void postprocessingReply(int fd)
{
	int i, fport;
	struct sockaddr_in fsin;
	socklen_t alen;
	getpeername(fd, (struct sockaddr *)&fsin, &alen);
	fport = ntohs(fsin.sin_port);

	for (i = 0; i < TABLE_LEN; i++) {
		if (ConnTable[i].isActive && ConnTable[i].port == fport) {
			time(&ConnTable[i].closeTm);
			printf("Time saved at close[%d] %s", i, ctime(&ConnTable[i].closeTm));
			close(fd); /* wait for 4min before reassigning slot to new conn */
			break;
		}
	}
	return ;
}

int TCPdaytimed(int fd)
{
	char *pts;
	time_t now;
	
	time(&now);
	pts = ctime(&now);
	write(fd, pts, strlen(pts));
	return 0;
}
