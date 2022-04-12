mysh: shell.c interpreter.c shellmemory.c pcb.c kernel.c cpu.c memorymanager.c test.c
	gcc -D FRAMESIZE=$(framesize) -D VARMEMSIZE=$(varmemsize) -c shell.c interpreter.c shellmemory.c pcb.c kernel.c cpu.c memorymanager.c test.c
	gcc -o mysh shell.o interpreter.o shellmemory.o pcb.o cpu.o kernel.o memorymanager.o test.o

clean: 
	rm mysh; rm *.o
