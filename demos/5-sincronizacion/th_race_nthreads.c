#include <stdio.h>
#include <pthread.h>

#define THREADS 10000

int counter = 0;

void *th_func(void *params) {
	int i;
	for(i = 0; i < 10000; ++i) {
		counter++;
	}
}

int main(void) {
	pthread_t th[THREADS];
	int i, j;

	for(j=0; j<30; ++j) {
		counter = 0;
	
		for(i=0; i<THREADS; ++i)
			pthread_create(&th[i], 0, th_func, 0);

		for(i=0; i<THREADS; ++i)
			pthread_join(th[i], 0);

		printf("\n\tcounter: %d", counter);
		if(counter != THREADS * 10000)
			printf("\n\t\tFALLO");
	}

	return 0;
}
