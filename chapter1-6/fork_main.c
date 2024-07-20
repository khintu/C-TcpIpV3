#include <stdio.h>
#include <unistd.h>

int sum;

int main (int argc, char* argv[]) 
{
	int i, pid, childnum;
	sum = 0;
	childnum = 0;

	if ((pid = fork()) == 0) /* child process */
	{
		printf("Sum in child process\n");
		goto childworker;
	}
	if ((pid = fork()) == 0) /* child process */
	{
		printf("Sum in child process\n");
		goto childworker;
	}
	if ((pid = fork()) == 0) /* child process */
	{
		printf("Sum in child process\n");
		goto childworker;
	}
	if ((pid = fork()) == 0) /* child process */
	{
		printf("Sum in child process\n");
		goto childworker;
	}
	if ((pid = fork()) == 0) /* child process */
	{
		printf("Sum in child process\n");
		goto childworker;
	}
childworker:
	if (pid == 0)
	{
		for (i = 1; i <= 3; ++i)
		{
			printf("The value of i is %d\n", i);
			fflush(stdout);
			sum += i;
		}
		printf("%d The sum is %d\n", pid, sum);
		sleep(10);
	}
	return 0;
}
