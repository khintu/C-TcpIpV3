#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <rpc/xdr.h>
#include <xdr_common_decl.h>

int errexit(const char *format, ...);
int passiveUDP(const char *service);
int recvUdp(int s, void* buffer, int nbytes);
int sendUdp(int s, void* buf, int nbytes);

struct sockaddr_in fsin;
int alen;

int main (int argc, char* argv[])
{
	struct MsgXdr m = {0};
  char *service = "10007";
  int msock;

  switch(argc) {
    case 1:
      break;
    case 2:
      service = argv[1];
      break;
    default:
      errexit("Usage: %s [port]\n", argv[0]);
  }
  msock = passiveUDP(service);

	while (1) {
		xDecodeRec2(msock, &m);
		printf("SRV# Msg Recevied from: %d %s (%d) %s\n", m.type, m.name, m.usr_id, m.number);
		m.type = 2; m.usr_id = 111178;m.name[0]=m.number[0]='\0'; 
		strcpy(m.name, "Taylor Bell");
		strcpy(m.number, "314-789-5432");
		xEncodeRec2(msock, &m);
		memset(&fsin, 0x0, sizeof fsin); alen = 0;
	}
	close(msock);
	return 0;
}

int recvUdp(int s, void* buf, int nbytes)
{
	int n;
	alen = sizeof fsin;
	fprintf(stderr, "DEBUG: recvUdp msock %d, nbyte %d, alen %d\n", s, nbytes, alen);
	if ((n = recvfrom(s, buf, nbytes, 0, (struct sockaddr *)&fsin, &alen)) < 0)
		errexit("error in recvfrom: %s\n", strerror(errno));
	fprintf(stderr, "DEBUG: recvUdp msock %d, nbyte %d, alen %d\n", s, n, alen);
	return n;
}

int sendUdp(int s, void* buf, int nbytes)
{
	int n;
	fprintf(stderr, "DEBUG: sendUdp msock %d, nbyte %d, alen %d\n", s, nbytes, alen);
	if ((n = sendto(s, buf, nbytes, 0, (struct sockaddr *)&fsin, alen)) < 0)
		errexit("error in sendto: %s\n", strerror(errno));
	return n;
}

int xEncodeRec2(int s, struct MsgXdr *m)
{
	XDR xdrs;
	char* p;
	xdrrec_create(&xdrs, sizeof (struct MsgXdr),\
											0,\
											(void*)s,\
											(int (*)(void *, void *, int))NULL,\
											(int (*)(void *, void *, int))sendUdp);
	xdrs.x_op = XDR_ENCODE;
	if (!xdr_int(&xdrs, &m->type))
		errexit("error writing m.type\n");
	if (!xdr_int(&xdrs, &m->usr_id))
		errexit("error writing m.usr_id\n");
	p = m->name;
	if (!xdr_string(&xdrs, (char**)&p, strlen(m->name)))
		errexit("error writing m.name\n");
	p = m->number;
	if (!xdr_string(&xdrs, (char**)&p, strlen(m->number)))
		errexit("error writing m.number\n");
	fprintf(stderr, "DEBUG: Ready to send m to socket\n");
	if (1 != xdrrec_endofrecord(&xdrs, 1))
		errexit("XDR record encode failed2\n");
	xdr_destroy(&xdrs);
	return 0;
}

int xDecodeRec2(int s, struct MsgXdr *m)
{
	XDR xdrs;
	char* p;
	xdrrec_create(&xdrs, 0,\
											sizeof (struct MsgXdr),\
											(void*)s,\
											(int (*)(void *, void *, int))recvUdp,\
											(int (*)(void *, void *, int))NULL);
	xdrs.x_op = XDR_DECODE;
	if (!xdrrec_skiprecord(&xdrs))
		errexit("error in skiprecord\n");
	if (!xdr_int(&xdrs, &m->type))
		errexit("error reading m.type\n");
	if (!xdr_int(&xdrs, &m->usr_id))
		errexit("error reading m.usr_id\n");
	p = m->name;
	if (!xdr_string(&xdrs, (char**)&p, 128))
		errexit("error reading m.name\n");
	p = m->number;
	if (!xdr_string(&xdrs, (char**)&p, 24))
		errexit("error reading m.number\n");
	if (1 != xdrrec_eof(&xdrs))
		errexit("XDR record decode failed2\n");
	xdr_destroy(&xdrs);
	return 0;
}
