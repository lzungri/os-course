#include <ucontext.h>
#include <stdio.h>

void main(void) {
	ucontext_t context;

	getcontext(&context);
	printf("loop - ");
	setcontext(&context);
}
