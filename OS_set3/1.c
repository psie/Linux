// testing dynamic loading of shared objects

#include <stdio.h>
#include <dlfcn.h>

int main()
{
	char versions[3], cmd[20];
	void *loc;
	void (*ptr)(char *);

	sprintf(cmd, "pmap %d", getpid());

	system(cmd);

	loc = dlopen("./libSDL2.so", RTLD_LAZY);

	if(!loc)
	{
		printf("%s\n", dlerror());
		return 1;
	}

	printf("After opening libSDL:\n");

	system(cmd);

	ptr = dlsym(loc, "SDL_GetVersion");

	if(!ptr)
	{
		printf("%s\n", dlerror());
		return 2;
	}

	ptr(versions);

	printf("libSDL%d.%d.%d\n", versions[0], versions[1], versions[2]);

	return 0;
}
