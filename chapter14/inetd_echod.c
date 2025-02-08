#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#define BUFFERSIZE	4096
int errexit(const char* format, ...);

int main (int argc, char* argv[]) 
{
	char buf[BUFFERSIZE];
	int cc;
	
	printf("Starting echod server from inetd\n");

	while (cc = read(0, buf, sizeof buf)) {
		if (cc < 0)
			errexit("echo read error: %s\n", strerror(errno));
		if (write(0, buf, cc) < 0)
			errexit("echo write error: %s\n", strerror(errno));
	}
	return 0;
}
