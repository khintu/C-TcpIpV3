#include <stdio.h>
#include <time.h>
#include <stdint.h>

int main (int argc, char* argv[])
{
	uint32_t t, y;

	for (t = 0, y = 1900 ; y < 1970 ; ++y) {
		if ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)
		{
			t += 60*60*24*366;
			printf("%u ", t);
		}
		else
			t += 60*60*24*365;
	}
	printf("\nUNIXEPOCH = %u\n", t);
	return 0;
}
