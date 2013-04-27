// operating on (large) files
// - using syscalls vs mapping a file into address space
// - operation: if(islower(c)) toupper(c); else tolower(c);

#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFSIZE 1024

int main(int argc, char *argv[])
{
	FILE *f;
	int fd, i;
	char buf[1024];
	char *ptr;
	int n, eof;
	unsigned t;
	struct stat s;
	long unsigned u;

	t = time(0);

	if(argc < 2)
	{
		printf("Please specify a file to test on.");
		return 0;
	}
	f = fopen(argv[1], "r+");

	if(!f)
		return 1;

	while(1)
	{
		n = fread(buf, 1, BUFSIZE, f);
		eof = feof(f);

		for(i=0; i<n; ++i)
		{
			if(buf[i] >= 'A' && buf[i] <= 'Z')
				buf[i] += 'a' - 'A';
			else //if(buf[i] >= 'a' && buf[i] <= 'z')
				buf[i] -= 'a' - 'A';
		}
		fseek(f, -n, SEEK_CUR);
		fwrite(buf, 1, n, f);
		if(eof)
			break;
	}

	fclose(f);

	printf("fread()/fwrite() has taken %us.\n", (unsigned)time(0)-t);

	t = time(0);

	fd = open(argv[1], O_RDWR);

	fstat(fd, &s);

	ptr = mmap(NULL, s.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	close(fd);

	for(u=0; u<s.st_size; ++u)
	{
		if(ptr[u] >= 'A' && ptr[u] <= 'Z')
			ptr[u] += 'a' - 'A';
		else//if(buf[i] >= 'a' && buf[i] <= 'z')
			ptr[u] -= 'a' - 'A';
	}

	//msync(ptr, s.st_size, MS_SYNC);

	munmap(ptr, s.st_size);

	printf("mmap() has taken %us.\n", (unsigned)time(0)-t);

	return 0;
}

