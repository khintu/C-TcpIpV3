#include <stdio.h>
#include <time.h>

int main (int argc, char* argv[]) 
{
	struct tm* lcl;
	time_t t;

	//time(&t);
	//t = 0xFFFFFFFF;
	//t = 0x80000000;
	//t = 0x7FFFFFFF;
	//t = -0.3f;
	//t = -1;
	//t = -2147483648;
	//t = 2147483647;
	time(&t);
	lcl = gmtime(&t);
	printf("UTC %s\n", asctime(lcl));
	printf("Local %s\n", ctime(&t));
	return 0;
}
