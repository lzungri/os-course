#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CANTIDAD_MB 100

void main(void) {
	int mem_alocada = 0;
	void *ptr;

	printf("\nAlocando memoria de a %dMiB:", CANTIDAD_MB);
	while(1) {
		ptr = malloc(CANTIDAD_MB*1024*1024);
		if(ptr < 0) {
			printf("Error al alocar memoria");
			break;
		}
		bzero(ptr, CANTIDAD_MB*1024*1024);

		mem_alocada += CANTIDAD_MB;
		printf("\n\tTotal alocado: %d megabytes", mem_alocada);
		sleep(1);
	}
}
