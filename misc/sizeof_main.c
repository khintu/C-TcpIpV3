#include <stdio.h>
#include <stddef.h>

int main (int argc, char* argv[]) {

	int arr[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};

	printf("Sizeof array = %zu\n", sizeof(arr));
	printf("Number of elements = %zu\n", sizeof(arr)/sizeof(arr[10]));
	return 0;
}
