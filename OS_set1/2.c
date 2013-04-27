// testing atexit()
// conclusions:
// - not called after abort(), SIGTERM
// - inherited from parent by default
// - stackable

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void parent_atexit()
{
	printf("atexit() from pid: %d\n", getpid());
}

void abort_atexit()
{
	puts("atexit() after abort()");
}

void exit_atexit()
{
	puts("atexit() after exit()");
}

void sigterm_atexit()
{
	puts("atexit() after SIGTERM");
}

int main()
{

atexit(parent_atexit);

int abort_pid = fork();
int exit_pid;
int sig_pid;

if(abort_pid == 0)
{
	exit_pid = fork();
	
	if(exit_pid == 0)
	{
		sig_pid = fork();

		if(sig_pid == 0)
		{
			atexit(sigterm_atexit);
			sleep(1);	
		}
		else
			kill(sig_pid, SIGTERM);
	
		atexit(exit_atexit);
		exit(1);
	}

	atexit(abort_atexit);
	abort();
}

return 0;
}
