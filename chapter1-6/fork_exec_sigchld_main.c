#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>


int main (int argc, char* argv[], char* env[]) 
{
	int pid, wstatus;
	//argv[1] = "-yup";
	if ((pid = fork()) == 0)
	{
		execve("/usr/bin/ls", argv, env);
	}

	/*waitpid(pid, &wstatus, 0);
	if (WIFEXITED(wstatus))
		printf("Child exit code %d\n", WEXITSTATUS(wstatus));
		*/
	return 0;
}
