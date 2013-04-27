// testing valgrind (memcheck)
// trying to trigger errors specified here: 
// http://valgrind.org/docs/manual/mc-manual.html#mc-manual.errormsgs

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	int a[4], *ptr, *ptr2 = malloc(1);
	char *b = (char*) malloc(10);
	
	printf("%d", a[0]); // use of uninitialised data
						// (jump or move based on uninitialised data)

	write(1, a, 1); // syscall with uninitialised data under memory pointer 
					// passed as argument
	dup(a[0]); // syscall with uninitialised data in argument

	memcpy(b, b+1, 12312); // source and destination overlap
						   // bounds exceding allocated memory boundary
						   // also invalid read/write

	free(b);
	free(b); // double free

	ptr = a;
	free(ptr); // invalid free

	//if in C++ we could try using invalid deallocator 
	// (eg. free() on memory allocated with new)

	//memory under ptr2 was not freed, lost block

	return 0;
}

