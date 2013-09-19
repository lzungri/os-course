#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

char *mem;

void *th_func(void * params) {
	mem = (char *) malloc(1000);
	printf("\nMemoria allocada: %X", mem);
	sleep(5);
	char *ptr=0;
	ptr[0]='a';
}

void *th_func2(void *params) {
	while(1) {
		printf("\n\tThread func2 en pie...");
		sleep(1);
	}
}


int main(void) {
	pthread_t th;
	pthread_t th2;

	printf("\nCreando thread...");
	pthread_create(&th, 0, th_func, 0);
	pthread_create(&th2, 0, th_func2, 0);
	printf("\nEsperando thread...");
	pthread_join(th, 0);
	pthread_join(th2, 0);
	free(mem);
	mem[0] = 'A';
	mem[1] = 0;

	printf("\nMem: %s", mem);

	printf("\nTerminando...");

	return 0;
}
