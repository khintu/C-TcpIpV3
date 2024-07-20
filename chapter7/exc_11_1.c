#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int connectTCP(const char *, const char *);
int errexit(const char *,...);
int TCPecho(const char *host, const char *service, const char *infile);

#define LINELEN	128

int main (int argc, char* argv[]) 
{
	char *infile = NULL;
	char* host = "localhost", *service = "echo";

	switch(argc) {
		case 1:
			//use default values
			break;
		case 4:
			infile = argv[3];
		case 3:
			service = argv[2];
		case 2:
			host = argv[1];
			break;
		default:
			fprintf(stderr, "usage: %s [host [port]]\n", argv[0]);
			exit(1);
	}
	TCPecho(host, service, infile);
	return 0;
}



int TCPecho(const char *host, const char *service, const char* infile)
{
	char buf[LINELEN+1];
	int s, n, wasread;
	int outchars, inchars;
	FILE* fptr = stdin;

	if ((s = connectTCP(host, service)) < 0)
		errexit("Could not connect to server\n");
	printf("Connection established...\n");
	while ((infile && (fptr = fopen(infile, "r"))) || !infile) {
		while(fgets(buf, sizeof buf, fptr)) {
			outchars = strlen(buf);
			//printf("Echo Sent[%d]: %s\n", outchars, buf);
			if (write(s, buf, outchars) < 0)
				errexit("Write error: %s\n", strerror(errno));
			
			for (wasread = 0,inchars = 0 ; inchars < outchars ; inchars += n) {
				n = read(s, &buf[inchars], outchars - inchars);
				if (n < 0) {
					if (errno == EAGAIN || errno == EWOULDBLOCK) {
						//printf("Nothing to read...Retry\n");
						usleep(300);
						n = 0;
					}
					else
						errexit("Read error: %s\n", strerror(errno));
				}
				else if (n == 0)
					errexit("Read: Connection closed by peer\n");
				else
					wasread = 1;
			}
			if (wasread){
				buf[inchars] = '\0';
				fputs(buf, stdout);
				//printf("Echo Reply[%d, n=%d]: %s\n", inchars,n, buf);
			}
		}
		if (infile != NULL) {
			fclose(fptr);
			sleep(1);
		}
		else
			break;
	}
	close(s);
	return 0;
}
