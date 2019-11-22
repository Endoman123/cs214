#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <math.h>

const char* SEARCH_TYPE = "process";

int search_proc(int[], int, int);

int search(int* arr, int length, int target, int maxSize) {
    int numFork = ceil((double) length / 250), pids[numFork], pid; 

    int i;
    for (i = 0; i < numFork; i++) {
        pid = fork();

        if (pid < 0) { // Case 1: Forking gives error
            printf("Error: error when creating new process\n"); 
        } else if (pid > 0) { // Case 2: Fork succeeds, this is the parent process
            pids[i] = pid;
        } else { // Case 3: Fork succeeds, this is the child process
            int offset = maxSize * i, 
                arrLen = maxSize + offset < length ? maxSize : length - offset - 1,
                ret = search_proc((arr + i * maxSize), arrLen, target);
            exit(ret);
        }
    }

    // TODO: this isn't actually a working fork waiter, rw this.
    int status, exitCode = 255; 
    for (i = 0; i < numFork; i++) {
        waitpid(pids[i], &status, 0);
        if (WIFEXITED(status)) exitCode = WEXITSTATUS(status);
        if (exitCode <= 250) return exitCode + maxSize * i;
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
