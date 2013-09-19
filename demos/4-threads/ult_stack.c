#include <ucontext.h>
#include <stdio.h>
#include <unistd.h>

char funcion1_stack[1024];

void funcion2(int param1, int param2) {
	printf("function2()\n");
	
	int param1_desde_stack;
	int param2_desde_stack;

}

void funcion1() {
	printf("function1()\n");

	funcion2(0xAAAAAA, 0xBBBBBB);
}

void main(void) {
	ucontext_t funcion1_ctx;

	getcontext(&funcion1_ctx);
	funcion1_ctx.uc_stack.ss_sp = funcion1_stack;
	funcion1_ctx.uc_stack.ss_size = sizeof(funcion1_stack);
	funcion1_ctx.uc_link = NULL;
	makecontext(&funcion1_ctx, funcion1, 0);

	setcontext(&funcion1_ctx);
}
