#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <xdr_common_decl.h>

int connectTCP(const char *, const char *);
int errexit(const char *,...);


int main (int argc, char* argv[])
{
	char buffer[sizeof(struct MsgXdr)] = {0};
	struct MsgXdr m = {0, 897678};
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

  s = connectTCP(host, service);
	strcpy(m.name, "Praveen Singh");
	strcpy(m.number, "213-314-4466");
	xEncodeMsg(buffer, &m);
  write(s, buffer, sizeof buffer);

  while ((n = read(s, buffer, sizeof buffer)) > 0){
		xDecodeMsg(buffer, &m);
		break;
	}
	printf("CLNT# Msg recvd: %d, %d, %s, %s\n", m.type, m.usr_id,\
					m.name, m.number);
	close(s);
	return 0;
}
