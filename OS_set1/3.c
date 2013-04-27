// using execve, checking exit code
#include <stdio.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	int pid = fork();
	int stat;
	if(pid == 0)
		execve(argv[1], &argv[1], NULL);
	else
	{
		wait(&stat);
		if(WIFEXITED(stat))
			printf("exited with %d code\n", WEXITSTATUS(stat));
		else if(WIFSIGNALED(stat))
			printf("died after receiving %d\n", WTERMSIG(stat));
	}
	return 0;
}
