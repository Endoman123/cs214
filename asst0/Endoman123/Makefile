all: compile

compile: check.c
	gcc -g check.c -o check

clean:
	rm -f check

test: compile
	@echo "Testing \"1 + 2; 2 + 3; true AND false\""
	@./check "1 + 2; 2 + 3; true AND false"
	@echo "Testing \"2 AND true\""
	@./check "2 AND true"
	@echo "Testing \"1 + + 2\""
	@./check "1 + + 2"
	@echo "Testing \"+ 3\""
	@./check "+ 3"
	@echo "Testing \"1 + 2;2 + 3\""
	@./check "1 + 2;2 + 3"
	@echo "Testing \"1 + 3;\""
	@./check "1 + 3;"
	@echo "Testing \"1+ 2\""
	@./check "1+ 2"
	@echo "Testing \"; ; \""
	@./check "; ; "
	@echo "Testing \";; \""
	@./check ";; "
	@echo "Testing \"1 + 2 3 + 4\""
	@./check "1 + 2 3 + 4"

debug: compile	
	gdb --args check "2 AND true"
