all:
	gcc loop.c -o loop -g -static
	gcc loop.c -o loop32 -g -static -m32
	gcc ult.c -o ult -g -static -m32
	gcc th_race.c -o th_race -lpthread
	gcc ult_stack.c -o ult_stack -g

enable_randomization:
	sudo sysctl kernel.randomize_va_space=2

disable_randomization:
	sudo sysctl kernel.randomize_va_space=0
