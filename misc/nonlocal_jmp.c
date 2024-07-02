#include <stdio.h>
#include <setjmp.h>

jmp_buf jmp1;

int factorial(int n)
{
	if (n == 0)
		return 1;
	else if (n == 11)
		// Free heap memory used if any before jmp
		longjmp(jmp1, 1);
	else if (n == 22)
		// Free heap memory used if any before jmp
		longjmp(jmp1, 2);
	else
		return n*factorial(n-1);
}

int glbn = 97;

// The point is that longjmp may restore registers from automatic variables and those defined register, so dont reply on new values set after setjmp, like in line 33 below. To force non-restore use volatile applicable for only local variables not global.


int main (int argc, char* argv[]) 
{
	int err, pn;
	volatile int n = 97;// The use of volatile will make them reliable after longjmp returns. They wont be rollbacked, new value will persist.
	if ((err = setjmp(jmp1)) == 0)
	{
		printf("Enter n: ");
		if (EOF != scanf("%d", &pn));
		n = pn;glbn = pn; //Dont rely on the new values of these two variables after setjmp returns from longjmp
		printf("fact=%d\n", factorial(pn));
	}
	else if (err == 1)
	{
		printf("Error from fact\n");
	}
	else
	{
		printf("Some unhandled error!!\n");
	}
	// There is no rollback of automatic/register variables on stack for main from where it called setjmp first time. There maybe a rollback so dont depend on values used for locals that you used after setjmp.
	printf("Value of n = %d\n", n);
	printf("Value of glbn = %d\n", glbn);
	return 0;
}
