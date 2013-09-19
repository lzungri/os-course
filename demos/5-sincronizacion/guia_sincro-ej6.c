#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

sem_t sem_a, sem_b, sem_c, sem_d;

void *thread_a(void *params) {
	while(1) {
		sem_wait(&sem_a);
		printf("A");
		sem_post(&sem_d);
	}
}

void *thread_b(void *params) {
	while(1) {
		sem_wait(&sem_b);
		sem_wait(&sem_d);
		printf("B");
		sem_post(&sem_a);
		sem_post(&sem_c);
	}
}

void *thread_c(void *params) {
	while(1) {
		sem_wait(&sem_c);
		sem_wait(&sem_d);
		printf("C");
		sem_post(&sem_a);
		sem_post(&sem_b);
	}
}

int main(void) {
	pthread_t th;

	sem_init(&sem_a, 0, 0);
	sem_init(&sem_b, 0, 1);
	sem_init(&sem_c, 0, 0);
	sem_init(&sem_d, 0, 1);

	pthread_create(&th, 0, thread_a, 0);
	pthread_create(&th, 0, thread_b, 0);
	pthread_create(&th, 0, thread_c, 0);

	usleep(1000);

	return 0;
}
