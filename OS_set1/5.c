// process (obviously) can't catch a SIGKILL
#include <stdio.h>
#include <signal.h>

struct sigaction intact;

void term_action(int sig)
{
	puts("Got SIGTERM!");
}

void int_action(int sig)
{
	puts("Got SIGINT!");
}

int main()
{
	int pid;

	pid = fork();

	if(pid == 0)
	{
		signal(SIGTERM, term_action);
		signal(SIGINT, int_action);
		fflush(stdout);
		while(1)
		sleep(1);
	}
	else
	{
		sleep(1);
		kill(pid, SIGTERM);
		kill(pid, SIGINT);
		sleep(1);
		printf("%d", kill(pid, SIGKILL));
	}

	return 0;
	}

