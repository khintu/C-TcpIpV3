#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <signal.h>

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
	char buf[LINELEN+1];
	int s, n, wasread, rdlast = 0;
	int outchars, inchars;
	
	//signal(SIGPIPE, SIG_IGN);

	if ((s = connectTCP(host, service)) < 0)
		errexit("Could not connect to server\n");
	printf("Connection established...\n");

	while(fgets(buf, sizeof buf, stdin)) {
		outchars = strlen(buf);
		//if (write(s, buf, outchars) < 0)
		if (send(s, buf, outchars, MSG_NOSIGNAL) < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				printf("Try again...buffer full");
				continue;
			}
			else
				errexit("Write error: %s\n", strerror(errno));
		}

		if (strcmp(buf, "Bye\n") == 0) {
			shutdown(s, SHUT_WR);
			rdlast = 1;
		}

		for (wasread = 0,inchars = 0 ; inchars < outchars ; inchars += n) {
			n = read(s, &buf[inchars], outchars - inchars);
			if (n < 0) {
				if (errno == EAGAIN || errno == EWOULDBLOCK) {
					printf("Nothing to read...Retry\n");
					usleep(300);
					n = 0;
				}
				else
					errexit("Read error: %s\n", strerror(errno));
			}
			else if (n == 0)
			{
				shutdown(s, SHUT_RDWR);
				errexit("Read: Connection closed by peer\n");
			}
			else
				wasread = 1;
		}

		if (wasread){
			buf[inchars] = '\0';
			fputs(buf, stdout);
			if (rdlast)
				break;
		}
	}
	close(s);

	return 0;
}
