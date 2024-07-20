#include <stdio.h>
#include <stdint.h>

uint8_t Vfd[1024 / 8];

#define FD_SET(fd)	{\
	uint8_t bm = 0x80;\
	bm >>= (fd % 8);\
	Vfd[fd / 8] |= bm;\
}

#define FD_CLR(fd) {\
	uint8_t bm = 0x80;\
	bm >>= (fd % 8);\
	Vfd[fd / 8] &= ~bm;\
}

#define FD_ISSET(fd) ((Vfd[fd / 8] >> (7 - (fd % 8))) ? 1 : 0)

#define FD_ZERO() {\
	for (int32_t i = 0 ; i < sizeof Vfd / sizeof Vfd[0]; ++i)\
		Vfd[i] = 0x0;\
}

int main (int argc, char* argv[]) 
{
	int32_t fdTest[6] = {1, 9, 17, 25, 33, 1023};
	FD_ZERO();
	for (int32_t i = 0; i < sizeof fdTest / sizeof fdTest[0]; ++i) {
		if (fdTest[i] > 1024-1){
			printf("Fd requested out of range\n");
			continue;
		}
		FD_SET(fdTest[i]);
		if (FD_ISSET(fdTest[i]))
			printf("%d tested positively for set\n", fdTest[i]);
		FD_CLR(fdTest[i]);
		if (!FD_ISSET(fdTest[i]))
			printf("%d tested positively for clear\n", fdTest[i]);
	}
	return 0;
}
