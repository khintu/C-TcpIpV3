#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>

int main (int argc, char* argv[]) 
{
	int s_fd;
	clock_t t1, t2;
	t1 = clock();
	//s_fd = socket(AF_INET, SOCK_STREAM, 0);
	//s_fd = socket(AF_INET, SOCK_DGRAM, 0);
	//s_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	//s_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	//s_fd = socket(PF_UNIX, SOCK_STREAM, 0);
	//s_fd = socket(AF_INET, SOCK_STREAM, 0);
	//s_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_ICMP);
	s_fd = socket(PF_ROUTE, SOCK_RAW, 0);
	if (s_fd < 0)
		perror("Socket create error");
	t2 = clock();
	//printf("time=%ld\n", (t2-t1)/CLOCKS_PER_SEC);
	printf("time=%ld\n", (t2-t1));
	close(s_fd);
	return 0;
}
