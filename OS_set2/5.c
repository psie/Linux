// Mesa monitor implementation

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define BUFSIZE 10

pthread_mutex_t mutex;
pthread_cond_t notfull, notempty;
int c;

void append()
{
	pthread_mutex_lock(&mutex);

	printf("%u is in append()\n\t", (unsigned)pthread_self());
	while(c == BUFSIZE - 1)
		pthread_cond_wait(&notfull, &mutex);

	printf("%u is adding something...", (unsigned)pthread_self());

	c++;

	printf("c is: %d\n", c);

	pthread_cond_signal(&notempty);

	printf("%u is leaving append()\n", (unsigned)pthread_self());

	pthread_mutex_unlock(&mutex);
}

void take()
{
	pthread_mutex_lock(&mutex);

	printf("%u is in take()\n\t", (unsigned)pthread_self());
	while(c == 0)
		pthread_cond_wait(&notempty, &mutex);

	printf("%u is taking something out...", (unsigned)pthread_self());

	c--;

	printf("c is: %d\n", c);

	pthread_cond_signal(&notfull);

	printf("%u is leaving take()\n", (unsigned)pthread_self());
	pthread_mutex_unlock(&mutex);
}

void* produce()
{
	while(1)
	{
		sleep(rand()%4);
		append();
	}
}

void* consume()
{
	while(1)
	{
		sleep(rand()%4);
		take();
	}
}

int main()
{
	int i;
	pthread_t producer[5];
	pthread_t consumer[5];

	srand(time(0));

	c = 0;
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&notfull, NULL);
	pthread_cond_init(&notempty, NULL);

	for(i=0; i<5; ++i)
	{
		pthread_create(&producer[i], NULL, &produce, NULL);
		pthread_create(&consumer[i], NULL, &consume, NULL);
	}

	for(i=0; i<5; ++i)
	{
		pthread_join(producer[i], NULL);
		pthread_join(consumer[i], NULL);
	}
}
	
