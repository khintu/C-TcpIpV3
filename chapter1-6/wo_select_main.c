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
	int fd, idx;
	char termID[L_ctermid], rdbuf[11] = {0}, cmd[11] = {0};
	
	printf("Term id %s\n", ctermid(termID));

	fd = open(termID, O_RDWR);

	while (1){
		printf("Reading...\n");
		if ((idx = read(fd, rdbuf, 10)) >= 0) 
		{
			strncpy(cmd, rdbuf, idx);
			cmd[idx-1] = 0;
		}
		else
		{
			printf("%s\n", strerror(errno));
			break;
		}

		printf("Writing...\n");
		if (write(fd, rdbuf, idx) < 0) 
		{	
			printf("%s\n", strerror(errno));
			break;
		}
		//printf("-%s-\n", cmd);
		if (strcmp(cmd, "Quit") == 0)
			break;
	}
	close(fd);
	return 0;
}
