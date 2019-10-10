all: memgrind

memgrind: clean mymalloc.o memgrind.c 
	gcc -g memgrind.c mymalloc.o -o memgrind

mymalloc.o: mymalloc.h mymalloc.c
	gcc -g -c mymalloc.c

clean:
	rm -rf memgrind mymalloc.o

test: memgrind
	./memgrind

debug: memgrind
	gdb memgrind
