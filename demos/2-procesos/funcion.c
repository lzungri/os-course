int funcion(int a, int b, int c) {
	return a;
}

int main(void) {
	return funcion(0xAAAAAAAA, 0xBBBBBBBB, 0xCCCCCCCC) + 1;
}
