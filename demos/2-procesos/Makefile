all:
	gcc simple_vars.c -g -o simple_vars32 -m32
	gcc simple_vars.c -g -o simple_vars
#	/opt/asm_debugger/edb --symbols simple_vars > /opt/asm_debugger/symbols/simple_vars.map
	gcc funcion.c -g -o funcion32 -O0 -m32
	gcc funcion.c -g -o funcion -O0
#	/opt/asm_debugger/edb --symbols funcion > /opt/asm_debugger/symbols/funcion.map
	gcc funcion_stack_params.c -g -o funcion_stack_params -fno-stack-protector
#	/opt/asm_debugger/edb --symbols funcion_stack_params > /opt/asm_debugger/symbols/funcion_stack_params.map
	gcc return_value.c -oreturn_value32 -g -m32
	gcc return_value.c -oreturn_value -g

	gcc parciales.c -o parciales -g -m32 -static
	gcc parciales.c -o parciales-nsp32 -g -m32 -fno-stack-protector
	gcc parciales.c -o parciales-nsp -g -fno-stack-protector
	
	gcc stack_ov.c -o stack_ov -g -m32
	gcc oomkiller.c -o oomkiller -g


enable_randomization:
	sudo sysctl kernel.randomize_va_space=2

disable_randomization:
	sudo sysctl kernel.randomize_va_space=0
