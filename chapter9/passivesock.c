#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

extern int errexit(const char *format,...);

u_short portbase = 0;	/* for non-root servers */


int passivesock(const char *service, const char *transport, const int qlen)
{
	struct servent *pse;
	struct protoent *ppe;
	struct sockaddr_in sin = {0};
	int s, type;
	
	
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;

	/* Service port number to bind to */
	if ((pse = getservbyname(service, transport)) != NULL)
		sin.sin_port = pse->s_port + htons(portbase);
	else if ((sin.sin_port = htons((u_short)atoi(service))) == 0)
		errexit("Service not in integer form '%s'\n", service);
	else 
		printf("Service name could not be resolved '%s', port number specified\n", service);

	/* Transport protocol/type of service for server socket */
	if ((ppe = getprotobyname(transport)) == NULL)
		errexit("Protocol specified '%s' incorrect\n", transport);

	if (strcmp(transport, "udp") == 0)
		type = SOCK_DGRAM;
	else
		type = SOCK_STREAM;
	
	/* Allocate a socket */
	if ((s = socket(PF_INET, type, ppe->p_proto)) < 0)
		errexit("Could not create socket: %s\n", strerror(errno));
	
	/* Bind socket to sin */
	if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		errexit("Can't bind to %s port: %s\n", service, strerror(errno));

	/* Set to passive by listen (TCP only) */
	if (type == SOCK_STREAM && listen(s, qlen) < 0)
		errexit("Can't listen on %s port: %s\n", service, strerror(errno));

	return s;
}
