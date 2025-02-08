#include <sys/resource.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define MAX_ITR		2000000000

void delayed_alloctn_cb(int);
void reaper(int);

jmp_buf env;

int main (int argc, char* argv[]) 
{
	int i, pid;
	struct sigaction sa = {0};

	sa.sa_handler = delayed_alloctn_cb;
	sigfillset(&sa.sa_mask);
	sa.sa_flags = SA_NODEFER;
	sigaction(SIGALRM, &sa, NULL);

	signal(SIGCHLD, reaper);
	//signal(SIGALRM, delayed_alloctn_cb);
	//setjmp(env);
	sigsetjmp(env, 1);
	for (i = 0 ; i <= MAX_ITR ; ++i) {
		
		if (i == MAX_ITR) {
			alarm(5);
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
	//longjmp(env, 1);
	siglongjmp(env,1);
	return ;
}

void reaper(int sig)
{
	int status, pid;
	while((pid = wait3(&status, WNOHANG, (struct rusage*)0)) > 0) {
		printf("-+Z+-Reaper read status %d, pid %d\n", WEXITSTATUS(status), pid);
	}
	return;
}
