#include <ucontext.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_ULTS 3

int ult_actual;

ucontext_t ult_contexts[MAX_ULTS];
// Se alocan los stacks en la seccion de datos (1MB de stack por cada thread).
char ult_stacks[MAX_ULTS][1 * 1024 * 1024];

// Estas funciones formarian parte de la biblioteca de ULTs.
//-----------------------------------------------------------------
void yield_to(int ult) {
	int ult_saliente = ult_actual;
	ult_actual = ult;

	printf("[Biblioteca ULTs]: ULT switch: Sale %d entra %d\n", ult_saliente, ult_actual);
	swapcontext(&ult_contexts[ult_saliente], &ult_contexts[ult_actual]);
	printf("[Biblioteca ULTs]: Restaurando contexto del ult %d\n", ult_actual);
}

void yield() {
	// Cede la ejecucion al proximo ULT.
	yield_to((ult_actual + 1) % MAX_ULTS);
}
//-----------------------------------------------------------------


void ult0() {
	printf("[ULT0]: Iniciando\n");
	while(1) {
		printf("[ULT0]: Dentro del while\n");
		sleep(3);
		yield();
	}
}

void ult1() {
	printf("[ULT1]: Iniciando\n");
	while(1) {
		printf("[ULT1]: Dentro del while\n");
		sleep(3);
		yield();
	}
}

void ult2() {
	int proximo_ult;

	printf("[ULT2]: Iniciando\n");
	while(1) {
		printf("[ULT2]: Dentro del while\n");
		do {
			printf("[ULT2]: Ingrese id del proximo ult a planificar [0-%d]: ", MAX_ULTS - 1);
			scanf("%d", &proximo_ult);
		} while(proximo_ult >= MAX_ULTS);

		yield_to(proximo_ult);
	}
}

void crear_contexto(int ult_id, void (*funcion_del_ult)()) {
	getcontext(&ult_contexts[ult_id]);
	ult_contexts[ult_id].uc_stack.ss_sp = ult_stacks[ult_id];
	ult_contexts[ult_id].uc_stack.ss_size = sizeof(ult_stacks[ult_id]);
	ult_contexts[ult_id].uc_link = NULL;
	makecontext(&ult_contexts[ult_id], funcion_del_ult, 0);
}

void main(void) {
	crear_contexto(0, ult0);
	crear_contexto(1, ult1);
	crear_contexto(2, ult2);

	ult_actual = 0 ;
	setcontext(&ult_contexts[ult_actual]);
}
