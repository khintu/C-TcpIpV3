#include <signal.h>
#include <unistd.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define MAX_ITR		2000000000

void delayed_alloctn_cb(int);

int main (int argc, char* argv[]) 
{
	int i, pid;
	
	signal(SIGALRM, delayed_alloctn_cb);
	alarm(5);
	for (i = 0 ; i <= MAX_ITR ; ++i) {
		
		if (i == MAX_ITR) {
			pid = fork();
			if (pid == 0){
				printf("processing child...exiting\n");
				break;
			}
			else if (pid < 0) {
				printf("parent fork error...exiting: %s\n", strerror(errno));
				exit(1);
			}
			else {

			}
			i = 0;
		}
	}

	return 0;
}


void delayed_alloctn_cb(int)
{
	printf("Alarm generated at 5 sec interval\n");
	alarm(5);
	return ;
}
