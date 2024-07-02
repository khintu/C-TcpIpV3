#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>


extern int h_errno;


int main (int argc, char* argv[]) 
{
	struct hostent *h;
	int i;
	clock_t t1, t2;

	t1 = clock();
	if (argv[1] && (h = gethostbyname(argv[1]))) {
		t2 = clock();
		printf("Official Name: %s\n", h->h_name);
		printf("Addr Type: %s\n", (h->h_addrtype == AF_INET)? "AF_INET": \
					(h->h_addrtype == AF_INET6)?"AF_INET6": "INVALID");
		printf("Aliases:\n");
		for (i = 0 ; h->h_aliases[i] ; ++i)
			printf("%s\n", h->h_aliases[i]);
		printf("IP addresses:\n");
		for (i = 0 ; h->h_addr_list[i] ; ++i)
			printf("%s\n", inet_ntoa(*((struct in_addr*)h->h_addr_list[i])));
		printf("Time taken = %ld\n", t2-t1);
		return 0;
	}
	else if (argv[1]) {
		printf("Resolve error for <%s>:%s\n", argv[1], hstrerror(h_errno));
	}
	else
		printf("Usage: %s <FQDN>\n", argv[0]);

	return 1;
}
