#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

int test_naming(int argc, char* argv[]) 
{
	int i;
	struct hostent *hptr;
	struct servent *sptr;
	struct protoent *pptr;
	//char *exnm = "merlin.cs.purdue.edu";
	//char *exnm = "ubuntu-server-vm";
	char *exnm = argv[1];
	char *snm = argv[2];
	char *pnm = argv[3];
	

	if (hptr = gethostbyname(exnm)){
		printf("%s ", hptr->h_name);
		for (i = 0; hptr->h_aliases[i] ; ++i){
			printf("%s\n", hptr->h_aliases[i]);
		}
		for (i = 0; hptr->h_addr_list[i] ; ++i){
			printf("%s\n", inet_ntoa(*((struct in_addr*)hptr->h_addr_list[i])));
		}
	}
	else
		herror("GetHostByName Error");

	if (sptr = getservbyname(snm, "tcp")) {
		printf("%s %d\n", sptr->s_name, ntohs(sptr->s_port));
	}
	else
		perror("GetServByName Error");

	if (pptr = getprotobyname(pnm)) {
		printf("%s %d\n", pptr->p_name, /*ntohs*/(pptr->p_proto));
	}
	else
		perror("GetProtoByName Error");
	return 0;
}

void sighdlr(int s)
{
	//printf("Broken pipe in write\n");
	if (s == SIGPIPE)
		perror("Write Error");
	exit(-1);
}

int main (int argc, char* argv[]) 
{
	int s_fd;
	struct sockaddr_in rem, lcl;
	char buf[1024] = {0};
	
	signal(SIGPIPE, sighdlr);

	inet_aton("127.0.0.1", &rem.sin_addr);
	rem.sin_family = AF_INET;
	rem.sin_port = htons(8893);

	inet_aton("127.0.0.1", &lcl.sin_addr);
	lcl.sin_family = AF_INET;
	lcl.sin_port = htons(18893);


	if ((s_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Socket Error");
		return -1;
	}
	
	/*if (bind(s_fd, (struct sockaddr *)&lcl, sizeof(lcl)) < 0)
	{
		perror("Local Bind Error");
		return -1;
	}*/
	if (connect(s_fd, (struct sockaddr *)&rem, sizeof rem) < 0) {
		perror("Connect Error");
		return -1;
	}
	printf("Connected to server...\n");
	
	socklen_t len = sizeof(lcl);
	if (getpeername(s_fd, (struct sockaddr *)&lcl, (socklen_t*)&len) < 0)
	{
		perror("Getsockname Error");
		return -1;
	}
	printf("Remote port (%d)\n", ntohs(lcl.sin_port));
	if (getsockname(s_fd, (struct sockaddr *)&lcl, (socklen_t*)&len) < 0)
	{
		perror("Getsockname Error");
		return -1;
	}
	printf("Local port bind successful (%d)\n", ntohs(lcl.sin_port));


	if (argv[1]) {
		if (write(s_fd, argv[1], strlen(argv[1])) < 0) {
			perror("Write Error");
			return -1;
		}
		printf("Sent to server\n");
	}
	close(s_fd);
	return 0;
}
