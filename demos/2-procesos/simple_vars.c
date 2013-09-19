#include <stdlib.h>

int global_var = 0xB0B0B0B0;
int main() {
	int local_var = 0xBBBBBBBB;
	int local_var2 = 0xCCCCCCCC;
	int *local_ptr;

	local_ptr = (int *) malloc(0xABCD);
	local_ptr[0] = 0xDDDDDDDD;
	
	return 0xABABABAB;
}
