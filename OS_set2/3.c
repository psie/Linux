// a resetting barrier implementation

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

// Let everybody through the frontdoor, tell the last to close it and open
// the backdoor, take everybody to the garden through it (one by one),
// tell the last person to close the backdoor and open the frontdoor,
// finally everybody leaves through it (one by one). 

struct barrier{
		int n, c;
		sem_t csem, frontsem, backsem;	
	} b;

void init_barrier(struct barrier *bar, int i)
{
	bar->n = i;
	bar->c = 0;
	sem_init(&bar->csem, 0, 1); // a binary semaphore protecting c
	sem_init(&bar->frontsem, 0, 1); // initially allow (everyone)
	sem_init(&bar->backsem, 0, 0); // initially allow noone
}

void wait_barrier(struct barrier *bar)
{
	sem_wait(&bar->csem);
		bar->c++; 

		if(bar->c == bar->n) // if everyone is ready
		{
			sem_wait(&bar->frontsem); // close the frontdoor
			sem_post(&bar->backsem); // one to unblock them all	
		}
	sem_post(&bar->csem);

	sem_wait(&bar->backsem); // here is the wait on the barrier
						   // once inside 
	sem_post(&bar->backsem); // let the next thread in
	
	sem_wait(&bar->csem);
		bar->c--;
	
		if(bar->c == 0) // the last 
		{
			sem_wait(&bar->backsem); // closes the backdoor 
			sem_post(&bar->frontsem); // and opens the frontdoor
		}
	sem_post(&bar->csem);

	sem_wait(&bar->frontsem); // get out
	sem_post(&bar->frontsem); // and let the next out
}

void destroy_barrier(struct barrier *bar)
{
	sem_destroy(&bar->csem);
	sem_destroy(&bar->frontsem);	
	sem_destroy(&bar->backsem);
}

void* pt_funtion(void *t)
{
	printf("%u here, see you in %ds\n", (unsigned)pthread_self(), (int)t);
	sleep((int)t);

	printf("%u is about to hit the barrier\n", (unsigned)pthread_self());
	wait_barrier(&b);

	printf("%u got beyond the barrier, lets repeat\n", (unsigned)pthread_self());

	sleep((int)t);
	
	printf("%u is about to hit the barrier\n", (unsigned)pthread_self());
	wait_barrier(&b);

	printf("%u got beyond the barrier\n", (unsigned)pthread_self());
		
	return;
}

int main()
{
	int i;
	pthread_t pt[5];

	init_barrier(&b, 5);
	
	for(i=0; i<5; ++i)
		pthread_create(&pt[i], NULL, *pt_funtion, (void*)i);

	for(i=0; i<5; ++i)
		pthread_join(pt[i], NULL);

	destroy_barrier(&b);

	return 0;
}
