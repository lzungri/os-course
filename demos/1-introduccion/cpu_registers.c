#include <stdio.h>

int variable = 0;
int main() {
	variable+=1;
	variable-=1;
	variable-=1;

	if(variable <= 5) {
		variable = 2;
	}

	if(variable <= 5) {
		variable = 3;
	}

	return 0;
}
