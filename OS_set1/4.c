// testing some features of process environmnet

#include <stdio.h>
#include <fcntl.h>

void printf_some_env()
{
	char cwd[256];

	printf("USER=%s\n", (char*) getenv("USER"));
	printf("LOGNAME=%s\n", (char*) getenv("LOGNAME"));
	printf("HOME=%s\n", (char*) getenv("HOME"));
	getcwd(cwd, 256);	
	printf("PWD=%s\n", cwd);
	printf("LANG=%s\n", (char*) getenv("LANG"));
	printf("PATH=%s\n", (char*) getenv("PATH"));
	printf("TERM=%s\n", (char*) getenv("TERM"));
	printf("EDITOR=%s\n", (char*) getenv("EDITOR"));
}

int main(int argc, char *argv[])
{
	int pid;
	int fd;
	char buf[256];

	printf_some_env();

	fd = open("4.c", O_RDWR);
		
	pid = fork();
	
	if(pid != 0)
	{
		setenv("EDITOR", "/usr/bin/vim", 1);
		//setenv("EDITOR", "/bin/rm -rf", 1);
		chdir("/tmp");
		puts("\nnew env:");
		printf_some_env();
	}
	else
	{
		sleep(1);
		puts("\nenv in child:");
		printf_some_env();

		puts("\nfirst 120 chars from file:");
		
		read(fd, buf, 120);
		buf[120]=0;
		printf("%s", buf);
	}
	
	if(pid != 0)
	{
		wait();
		puts("\n\nnext 120 chars:");

		read(fd, buf, 120);
		buf[120]=0;
		printf("%s\n", buf);
	}
	
	return 0;
}
	
	
