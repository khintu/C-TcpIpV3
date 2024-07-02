#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#ifndef INADDR_NONE
#define INADDR_NONE	0xffffffff
#endif /* INADDR_NONE */

extern int errexit(const char *format,...);


int connect_udp(int s,  struct sockaddr_in* sin)
{
	if (connect(s, (struct sockaddr *)sin, sizeof *sin) < 0)
		return 1;
	return 0;
}

int try_connect_tcp(int s, struct sockaddr_in* sin)
{
	int s_err;

	struct timeval tv;
	fd_set fdset;
	FD_ZERO(&fdset);
	tv.tv_sec = 10;             /* 10 second timeout */
	tv.tv_usec = 0;

	if (connect(s, (struct sockaddr *)sin, sizeof *sin) < 0)
		if (errno != EINPROGRESS)
			return 1;

	FD_SET(s, &fdset);
	if ((s_err = select(s + 1, NULL, &fdset, NULL, &tv)) == 1)
	{
		int so_error;
		socklen_t len = sizeof so_error;

		getsockopt(s, SOL_SOCKET, SO_ERROR, &so_error, &len);

		if (so_error != 0)
		{
			printf("Connect Error:%s\n", strerror(so_error));
			return 1;
		}
		printf("Connect returned successfully\n");
	}
	else if (s_err == 0) {
		printf("Can't connect to host, timeout...\n");
		return 1;
	}
	else {
		printf("Select error:%s\n", strerror(errno));
		return 1;
	}
	
	return 0;
}	

int connectsock(const char *host, const char *service, const char *transport)
{
	struct hostent *phe;
	struct servent *pse;
	struct protoent *ppe;
	struct sockaddr_in sin = {0};
	int s, type, c_err;
	
	
	sin.sin_family = AF_INET;

	if ((phe = gethostbyname(host)) != NULL) {
		memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
	}
	else if ((sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE)
		errexit("Hostname not in dotted decimal notation '%s'\n", host);
	else {
		printf("Hostname '%s' could not be resolved: %s\n", host, hstrerror(h_errno));
	}
	
	if ((pse = getservbyname(service, transport)) != NULL)
		sin.sin_port = pse->s_port;
	else if ((sin.sin_port = htons((u_short)atoi(service))) == 0)
		errexit("Service not in integer form '%s'\n", service);
	else 
		printf("Service name could not be resolved '%s'\n", service);

	if ((ppe = getprotobyname(transport)) == NULL)
		errexit("Protocol specified '%s' incorrect\n", transport);

	if (strcmp(transport, "udp") == 0)
		type = SOCK_DGRAM;
	else
		type = SOCK_STREAM | SOCK_NONBLOCK;

	if ((s = socket(PF_INET, type, ppe->p_proto)) < 0)
		errexit("Could not create socket: %s\n", strerror(errno));
	
	/* Timeout for connect to be set */
	//fcntl(s, F_SETFL, O_NONBLOCK);
	
	if (type == SOCK_DGRAM)
		c_err = connect_udp(s, &sin);
	else 
		c_err = try_connect_tcp(s, &sin);

	if (c_err == 0)
		printf("%s:%s is open\n", host, service);
	else
		errexit("Could not connect to host '%s', port '%s'\n", host, service);
	return s;
}
