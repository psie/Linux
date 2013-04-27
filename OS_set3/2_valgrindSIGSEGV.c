// my valgrind caught a SIGSEGV on that

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	int a[4], *ptr;
	char *b = (char*) malloc(10);
	
	printf("%d", a[0]);

	write(1, a, 1);
	dup(a[0]);

	memcpy(b, b+1, 12312);

	ptr = a;

	free(b);
	free(b);
	free(ptr);

	ptr = malloc(1);

	return 0;
}

