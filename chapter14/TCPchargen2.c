#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int connectTCP(const char *, const char *);
int errexit(const char *,...);
int TCPecho(const char *host, const char *service);

#define LINELEN	128

int main (int argc, char* argv[]) 
{
	char* host = "localhost", *service = "echo";

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
	TCPecho(host, service);
	return 0;
}



int TCPecho(const char *host, const char *service)
{
	char buf[LINELEN+1] = {0};
	int s;
	int n, wasread, count;

	if ((s = connectTCP(host, service)) < 0)
		errexit("Could not connect to server\n");
	printf("Connection established...\n");
	
	count = 5;
	while(count > 0) {
		wasread = 0;
		if (write(s, buf, 1) < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				usleep(300);
				continue;
			}
			else if (errno == EINTR){
				printf("Write Interrupted, bailing out\n");
				break;
			}
		}
label1:
		n = read(s, buf, sizeof buf);
		if (n < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				usleep(300);
				goto label1;
			}
			else if (errno == EINTR) {
				printf("Read Interrupted, bailing out\n");
				break;
			}
			else
				errexit("Read error: %s\n", strerror(errno));
		}
		else if (n == 0)
			errexit("Read: Connection closed by peer\n");
		else
			wasread = 1;
		if (wasread){
			buf[n-1] = '\0';
			//fputs(buf, stdout);
			printf("Bytes recevied %d, [%s]\n", n, buf);
		}
		count--;
	}
	printf("\nClosing connection to server\n");
	close(s);

	return 0;
}
