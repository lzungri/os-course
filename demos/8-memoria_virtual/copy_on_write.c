#include <unistd.h>
#include <stdio.h>

#define SIZE 1024*1024

int main() {
	char vector[SIZE];
	int pid;

	bzero(vector, SIZE);

	pid = fork();

	if(pid == 0) {
		printf("HIJO: Durmiendo...\n");
		sleep(20);
		printf("HIJO: Escribiendo memoria...\n");
		bzero(vector, SIZE);
		sleep(100);
	} else {
		sleep(1000);
	}
	

	return 0;
}
