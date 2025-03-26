#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <rpc/xdr.h>
#include <xdr_common_decl.h>

int connectUDP(const char *, const char *);
int errexit(const char *,...);

int main (int argc, char* argv[])
{
	struct MsgXdr m = {1, 897678};
  char* host = "localhost", *service = "10007";
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
	strcpy(m.name, "Praveen Singh");
	strcpy(m.number, "213-314-4466");
	xEncodeRec(s, &m);
	xDecodeRec(s, &m);
	printf("CLNT# Msg recvd: %d, %d, %s, %s\n", m.type, m.usr_id,\
					m.name, m.number);
	close(s);
	return 0;
}

int xEncodeRec(int s, struct MsgXdr *m)
{
	XDR xdrs;
	char* p;
	xdrrec_create(&xdrs, sizeof (struct MsgXdr),\
											0,\
											(void*)s,\
											(int (*)(void *, void *, int))NULL,\
											(int (*)(void *, void *, int))write);
	xdrs.x_op = XDR_ENCODE;
	xdr_int(&xdrs, &m->type);
	xdr_int(&xdrs, &m->usr_id);
	p = m->name;
	xdr_string(&xdrs, (char**)&p, strlen(m->name));
	p = m->number;
	xdr_string(&xdrs, (char**)&p, strlen(m->number));
	if (1 != xdrrec_endofrecord(&xdrs, 1))
		errexit("XDR record encode failed\n");
	xdr_destroy(&xdrs);
	return 0;
}

int xDecodeRec(int s, struct MsgXdr *m)
{
	XDR xdrs;
	char* p;
	xdrrec_create(&xdrs, 0,\
											sizeof (struct MsgXdr),\
											(void*)s,\
											(int (*)(void *, void *, int))read,\
											(int (*)(void *, void *, int))NULL);
	xdrs.x_op = XDR_DECODE;
	xdrrec_skiprecord(&xdrs);
	xdr_int(&xdrs, &m->type);
	xdr_int(&xdrs, &m->usr_id);
	p = m->name;
	xdr_string(&xdrs, (char**)&p, 128);
	p = m->number;
	xdr_string(&xdrs, (char**)&p, 24);
	if (1 != xdrrec_eof(&xdrs))
		errexit("XDR record decode failed\n");
	xdr_destroy(&xdrs);
	return 0;
}
