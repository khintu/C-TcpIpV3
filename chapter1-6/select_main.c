#include <stdio.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main (int argc, char* argv[]) 
{
	int fd, idx, s;
	fd_set rdfds;
	struct timeval ts;
	char termID[L_ctermid], rdbuf[11] = {0}, cmd[11] = {0};
	
	FD_ZERO(&rdfds);
	ts.tv_sec = 5;
	ts.tv_usec = 2000;

	printf("Term id %s\n", ctermid(termID));

	fd = open(termID, O_RDWR);
	FD_SET(fd, &rdfds);
	while (s = select(fd+1, &rdfds, NULL, NULL, &ts) >= 0) {
		if (s == 0) {
			printf("Nothing to read...\n");
		}
		else
			printf("Waking up from select ready to read %d\n", s);
		//break;
		for (int ifd = 0 ; ifd < fd+1 ; ++ifd) {
			if (FD_ISSET(ifd, &rdfds)) {
					printf("Socket ready %d\n", ifd);
				printf("Reading %d...\n", ifd);
				if ((idx = read(ifd, rdbuf, 10)) >= 0) {
					strncpy(cmd, rdbuf, idx);
					cmd[idx-1] = 0;
				}
				else {
					printf("%s\n", strerror(errno));
					break;
				}

				printf("Writing %d...\n", ifd);
				if (write(ifd, rdbuf, idx) < 0) {	
					printf("%s\n", strerror(errno));
					break;
				}
				//printf("-%s-\n", cmd);
				if (strcmp(cmd, "Quit") == 0)
					break;
			}
		}
		if (strcmp(cmd, "Quit") == 0)
			break;
	}
	if (s < 0)
		perror("Select Error");
	close(fd);
	return 0;
}
