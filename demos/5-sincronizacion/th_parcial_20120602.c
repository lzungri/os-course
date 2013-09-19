#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

sem_t sem_th2, sem_th3, sem_th4, sem_mutex, sem_v100;
int v = 0;

void *th_4(void *params) {
	while(1) {
		sem_wait(&sem_mutex);
		v++;
		sem_post(&sem_mutex);
	}
}

void *th_5(void *params) {
	while(1) {
		sem_wait(&sem_mutex);
		v++;
		sem_post(&sem_mutex);
	}
}

void *th_3(void *params) {
	sem_wait(&sem_v100);
	printf("\n\n--- v es 100 ---\n\n");
	fflush(0);
}

void *th_2(void *params) {
	pthread_t th;
	pthread_create(&th, 0, th_3, 0);
	printf("Thread 3->");
	fflush(0);
	sem_post(&sem_th3);
	sem_wait(&sem_th4);
	pthread_create(&th, 0, th_5, 0);
	printf("Thread 5->");
	fflush(0);
}

void *th_1(void *params) {
	pthread_t th;
	sem_wait(&sem_th3);
	pthread_create(&th, 0, th_4, 0);
	printf("Thread 4->");
	fflush(0);
	sem_post(&sem_th4);
}


int main(void) {
	pthread_t th;

	sem_init(&sem_th2, 0, 0);
	sem_init(&sem_th3, 0, 0);
	sem_init(&sem_th4, 0, 0);
	sem_init(&sem_mutex, 0, 1);
	sem_init(&sem_v100, 0, 0);


	pthread_create(&th, 0, th_1, 0);
	printf("Thread 1->");
	fflush(0);
	pthread_create(&th, 0, th_2, 0);
	printf("Thread 2->");
	fflush(0);

	while(1) {
		sem_wait(&sem_mutex);
		v++;
		if(v >= 10000) {
			sem_post(&sem_mutex);
			sem_post(&sem_v100);
			sleep(100000);
		} else {
			sem_post(&sem_mutex);
		}
	
	}


	return 0;
}
