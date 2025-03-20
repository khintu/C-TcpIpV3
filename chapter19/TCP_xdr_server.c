#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <xdr_common_decl.h>

int errexit(const char *format, ...);
int passiveTCP(const char *service, const int qlen);
int TCPdaytimed(int fd);
int sendNBytes(int s, void *buf, int nchars);
int recvNBytes(int s, void *buf, int nchars);

#define QLEN  5

int main (int argc, char* argv[])
{
	struct MsgXdr m = {0};
	char buffer[sizeof(struct MsgXdr)] = {0};
  struct sockaddr_in fsin;
  char *service = "10007";
  int msock, ssock;
  int alen, n;

  switch(argc) {
    case 1:
      break;
    case 2:
      service = argv[1];
      break;
    default:
      errexit("Usage: %s [port]\n", argv[0]);
  }
  msock = passiveTCP(service, QLEN);

  while (1) {
    ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
    if (ssock < 0)
      errexit("Accept failed: %s\n", strerror(errno));
    while ((n = recvNBytes(ssock, buffer, sizeof buffer)) > 0) {
			xDecodeMsg(buffer, &m);
			printf("SRV# Msg Recevied from: %d %s (%d) %s\n", m.type, m.name, m.usr_id, m.number);
			m.type = 1; m.usr_id = 911178;m.name[0]=m.number[0]='\0'; 
			strcpy(m.name, "Keira Lhotan");
			strcpy(m.number, "643-789-5432");
			xEncodeMsg(buffer, &m);
			sendNBytes(ssock, buffer, sizeof buffer);
			break;
		}
		close(ssock);
  }
	return 0;
}
