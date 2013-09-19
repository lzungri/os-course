#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

sem_t sem_a, sem_b;

void *thread_a(void *params) {
	int i=0;
	for(i=0; i<1000;++i) {
		sem_wait(&sem_a);
		sem_wait(&sem_a);
		printf("A");
		sem_post(&sem_b);
		sem_post(&sem_b);
	}
}

void *thread_b(void *params) {
	int i=0;
	for(i=0; i<1000;++i) {
		sem_wait(&sem_b);
		sem_wait(&sem_b);
		printf("B");
		sem_post(&sem_b);
		sem_post(&sem_a);
	}
}

int main(void) {
	pthread_t th;

	sem_init(&sem_a, 0, 2);
	sem_init(&sem_b, 0, 1);

	printf("\n");

	pthread_create(&th, 0, thread_a, 0);
	pthread_create(&th, 0, thread_b, 0);

	//usleep(1000);
	pthread_join(th, 0);

	return 0;
}
