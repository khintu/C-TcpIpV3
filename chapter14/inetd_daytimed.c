#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main (int argc, char* argv[]) 
{
	char *pts;
	time_t now;

	time(&now);
	pts = ctime(&now);
	write(0, pts, strlen(pts));
	return 0;
}
