// creating a zombie
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

int main()
{
	int pid = fork();
	if(pid == -1)
		exit(errno);

	if(pid == 0)
		sleep(1);
	else
	{
		if(kill(pid, SIGTERM)==-1)
		exit(errno);
		system("/bin/ps | grep defunct");
	}	
	return 0;
	
}
