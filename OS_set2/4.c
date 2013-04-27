// inter-process communicaton with shared memory and semaphores

#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define N 128
#define BUFSIZE 256

// mailbox  
// in this solution if there is no space old messages are overwritten
// moreover, there is no guarantee of FIFO order in receiving

struct mail
{
	char msg[BUFSIZE];
	int dest;
	int src;
};

struct shared
{
	int i; // currently "free" mail slot
	sem_t sem;
	struct mail mailbox[N];
} *shm;

void mb_init()
{
	shm->i = 0;
	sem_init(&shm->sem, 1, 1);
}

int receive(char *msg, int source)
{
	int j;

	sem_wait(&shm->sem);
		for(j=0; j < N; ++j) // no FIFO guarantee here
			if(shm->mailbox[j].dest == getpid() && shm->mailbox[j].src == source)
			{
				strncpy(msg, shm->mailbox[j].msg, BUFSIZE);
				
				memset(shm->mailbox[j].msg, 0, BUFSIZE); // discard read msg
				shm->mailbox[j].dest = 0;
				shm->mailbox[j].src = 0;

				sem_post(&shm->sem);
				return 0;
			}

	sem_post(&shm->sem);
	return -1; // no mail
}

void send(char *msg, int destination)
{
	sem_wait(&shm->sem);

		if(shm->i == N) // round buffer
			shm->i=0;
		
		shm->mailbox[shm->i].src = getpid();
		shm->mailbox[shm->i].dest = destination;
		strncpy(shm->mailbox[shm->i].msg, msg, BUFSIZE);

		shm->i++;

	sem_post(&shm->sem);
}

int main()
{
	int d, pid[5];
	char buf[BUFSIZE];

	d = shm_open("/mailbox", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

	ftruncate(d, sizeof(struct shared)); 

	shm = mmap(NULL, sizeof(struct shared), PROT_READ | PROT_WRITE, MAP_SHARED, d, 0);

	mb_init();

	pid[0] = fork();
	if(pid[0] != 0)
		pid[1] = fork();
	
	if(pid[0] == 0)
	{
		send("hello", getppid());
		printf("%d here, I just sent something to %d\n", getpid(), getppid());
	}
	else if(pid[1] == 0)
	{
		send("hi there", getppid());
		printf("%d here, I just sent something to %d\n", getpid(), getppid());

		sleep(2);
		
		printf("%d here, is there anything from %d?\n", getpid(), getppid()); 
		if(receive(buf, getppid()) == 0)
			printf("Received: %s\n", buf);
	}
	else
	{
		sleep(1);
		printf("%d here, is there anything from %d?\n", getpid(), pid[0]);
		if(receive(buf, pid[0]) == 0)
			printf("Received: %s\n", buf);
		printf("%d here, is there anything from %d?\n", getpid(), pid[1]);
		if(receive(buf, pid[1]) == 0)
			printf("Received: %s\n", buf);
		
		send("welcome, my son", pid[1]);
		printf("%d here, I just sent something to %d\n", getpid(), pid[1]);

		sleep(5);
	}

	return 0;
}
