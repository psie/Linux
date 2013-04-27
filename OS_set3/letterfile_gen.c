#include <stdio.h>

int main(int argc, char *argv[])
{
	if(argc < 2)
		return 0;

	//srand(time(0));

	int *f = fopen("test", "a");
	int i;
	for(i=0; i<atoi(argv[1]); ++i)
		fputc(rand()%2? 'a'+rand()%26 : 'A'+rand()%26, f);

	fclose(f);

}
