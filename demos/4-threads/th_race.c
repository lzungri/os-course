#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int counter = 0;

void *th_func(void *params) {
	int i;
	for(i = 0; i < 10000; ++i) {
		counter++;
	}
}

int main(void) {
	pthread_t th;

	printf("Creando thread...\n");
	pthread_create(&th, 0, th_func, 0);
	th_func(0);

	printf("Esperando thread...\n");
	pthread_join(th, 0);

	printf("Terminando...\n");
	printf("\tcounter: %d\n\n", counter);

	return 0;
}
