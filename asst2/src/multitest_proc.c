#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <math.h>

const char* SEARCH_TYPE = "process";

int sequentialSearch(int[], int, int);

int search(int* arr, int length, int target, int maxSize) {
    int numFork = ceil((double) length / maxSize), pids[numFork], pid, i;

    for (i = 0; i < numFork; i++) {
        pid = fork();

        if (pid < 0) { // Case 1: Forking gives error
            printf("Error: error when creating new process\n"); 
        } else if (pid > 0) { // Case 2: Fork succeeds, this is the parent process
            pids[i] = pid;
        } else { // Case 3: Fork succeeds, this is the child process
            int offset = maxSize * i, 
                searchLen = maxSize + offset < length ? maxSize : length - offset,
                ret = sequentialSearch((arr + offset), searchLen, target);
            
            exit(ret);
        }
    }
    
    // Wait for child pid here
    int ret = -1, status, exitCode = 255; 
    for (i = 0; i < numFork; i++) {
        waitpid(pids[i], &status, 0);

        if (WIFEXITED(status)) 
            exitCode = WEXITSTATUS(status);
        
        if (exitCode <= 250) 
            ret = exitCode + maxSize * i;
    }

    return ret;
}

int sequentialSearch(int* arr, int length, int target) {
    int i = 0;
    for (; i < length; i++) {
        if (arr[i] == target)
            return i;
    }    

	return -1; 
}
