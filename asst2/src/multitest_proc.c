#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <math.h>

const int MAX_PROCESS_SIZE = 250;
const char* SEARCH_TYPE = "process";

int search_proc(int[], int, int);

int search(int* arr, int length, int target) {
    int numFork = ceil((double) length / 250), pids[numFork], pid; 

    int i;
    for (i = 0; i < numFork; i++) {
        pid = fork();

        if (pid < 0) { // Case 1: Forking gives error
            printf("Error: error when creating new process"); 
        } else if (pid > 0) { // Case 2: Fork succeeds, this is the parent process
            pids[i] = pid;
        } else { // Case 3: Fork succeeds, this is the child process
            int offset = MAX_PROCESS_SIZE * i, 
                arrLen = MAX_PROCESS_SIZE + offset < length ? MAX_PROCESS_SIZE : length - offset - 1,
                ret = search_proc((arr + i * MAX_PROCESS_SIZE), arrLen, target);
            exit(ret);
        }
    }

    // TODO: this isn't actually a working fork waiter, rw this.
    int status, exitCode = 255; 
    for (i = 0; i < numFork; i++) {
        waitpid(pids[i], &status, 0);
        if (WIFEXITED(status)) exitCode = WEXITSTATUS(status);
        if (exitCode <= 250) return exitCode + MAX_PROCESS_SIZE * i;
    }
    return -1;
}


int search_proc(int* arr, int length, int target) {
    int i = 0;
    for (; i < length; i++) {
        if (arr[i] == target)
            return i;
    }    

	return -1; 
}
