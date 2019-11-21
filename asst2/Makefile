all:
	@echo Please use the 'proc' or 'thread' directive.

proc: bin/multitest_proc.o src/searchtest.c
	gcc -g -lm src/searchtest.c bin/multitest_proc.o -o search_proc 

thread: bin/multitest_thread.o src/searchtest.c
	gcc -g -lm -lpthread src/searchtest.c bin/multitest_thread.o -o search_thread

bin/multitest_proc.o: src/multitest_proc.c
	@mkdir -p bin
	gcc -g -c src/multitest_proc.c -o bin/multitest_proc.o

bin/multitest_thread.o: src/multitest_thread.c
	@mkdir -p bin
	gcc -g -c src/multitest_thread.c -o bin/multitest_thread.o

clean:
	rm -rf bin
	rm -f search_proc
	rm -f search_thread
	rm -f *_data.csv
