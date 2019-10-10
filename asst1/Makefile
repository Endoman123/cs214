all: compile

compile: memgrind.c mymalloc.o
	gcc -o memgrind memgrind.c mymalloc.o 

mymalloc.o: mymalloc.c mymalloc.h
	gcc -o mymalloc -c mymalloc.c -I

clean: 
	rm memgrind.out; rm mymalloc.o