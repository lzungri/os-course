#include <stdio.h>
#include <pthread.h>

int counter = 0;

void *th_func(void *params) {
	int i;
	for(i = 0; i < 10000; ++i) {
		counter++;
	}
}

int main(void) {
	pthread_t th;

	pthread_create(&th, 0, th_func, 0);
	th_func(0);

	printf("\nEsperando threads...");
	pthread_join(th, 0);

	printf("\n\tcounter: %d\n", counter);

	return 0;
}
