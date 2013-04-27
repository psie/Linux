#include <stdio.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

struct shared
{
	int initialised;
	int i;
	sem_t i_sem;
	sem_t w_sem;
} *shared_mem;

void reader(int t)
{
	sem_wait(&shared_mem->i_sem);
	++shared_mem->i;
	if(shared_mem->i == 1) // first reader holds the semaphore
		sem_wait(&shared_mem->w_sem);
	sem_post(&shared_mem->i_sem);
	
	printf("%u started reading...\n", getpid());
	sleep(t); // reading
	printf("%u finished reading after %ds\n", getpid(), t);

	sem_wait(&shared_mem->i_sem);
	--shared_mem->i;
	if(shared_mem->i == 0) // last reader releases the semaphore
		sem_post(&shared_mem->w_sem);
	sem_post(&shared_mem->i_sem);
	return;
}

void writer(int t)
{
	// each writer has to have exclusive access
	sem_wait(&shared_mem->w_sem);
	
	printf("%u started writing...\n", getpid());
	sleep(t); // writing
	printf("%u finished writing after %ds\n", getpid(), t);
	
	sem_post(&shared_mem->w_sem);
	return;
}

int main(int argc, char *argv[])
{
	int descriptor;

	descriptor = shm_open("/shm", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	// S_IRUSR | S_IWUSR - read/write permissions

	ftruncate(descriptor, sizeof(struct shared));

	shared_mem = mmap(NULL, sizeof(struct shared), PROT_READ | PROT_WRITE,
			MAP_SHARED, descriptor, 0);

	if(!shared_mem->initialised) // initialisation 
	{
		shared_mem->initialised = 1;
		shared_mem->i=0;
		sem_init(&shared_mem->w_sem, 1, 1);
		sem_init(&shared_mem->i_sem, 1, 1);
	}

	if(argc < 3)
	{
		puts("Please provide arguments:");
		puts("argv[1]: 1 - reader 2 - writer");
		puts("argv[2]: time to spend reading/writing.");
		return 0;
	}

	if(atoi(argv[1]) == 1)
		reader(atoi(argv[2]));
	else
		writer(atoi(argv[2]));
	
	return 0;
}

