// deteched threads do not need to store any information after
// they die

#include <stdio.h>
#include <pthread.h>

void *thread_entry(void *arg)
{
	printf("Thread %u here.\n", (unsigned)pthread_self());
	sleep(2);
	pthread_exit(0);
}

int main()
{
	int i;
	pthread_t pt[10];

	for(i=0; i<10; ++i)
		pthread_create(&pt[i], NULL, thread_entry, NULL);
	
	sleep(1);
	system("ps -L");

	for(i=0; i<10; ++i)
		pthread_join(pt[i], NULL);
	
	return 0;
}	
