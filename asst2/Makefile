all:
	@echo Please use the 'proc' or 'thread' directive.

proc: bin/multitest_proc.o src/searchtest.c
	gcc src/searchtest.c bin/multitest_proc.o -o search_proc 

bin/multitest_proc.o: src/multitest_proc.c
	@mkdir -p bin
	gcc -lm -c src/multitest_proc.c -o bin/multitest_proc.o

clean:
	rm -rf bin
	rm -f search_proc
	rm -f search_thread
