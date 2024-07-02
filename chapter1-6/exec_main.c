#include <stdio.h>
#include <unistd.h>


int main (int argc, char* argv[], char* env[]) 
{
	int pid;

	if ((pid = fork()) == 0)
	{
		execve("/usr/bin/ls", argv, env);
	}

	return 0;
}
