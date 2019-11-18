import <stdlib.h>
import <unistd.h>
import <math.h>

const int MAX_PROCESS_SIZE = 250;

int search(int arr[], int arrLen, int target) {
	int numProcs = ceiling( (float) arrLen / 250);
	int procs[numProcs], i;
	
	//Create the children. Let's put the pids into an array so we can call wait on them later.
	for (i = 0; i < numProcs; i++) {
		int pid = fork();
		if (pid > 0) {
			procs[i] = pid;
		} else { //The child will get a pid of 0, so it will go to this if. Let's have it search. 
			int cpid = getpid(); //THe current process is the child, get its pid.
			int multiplier = 0;
			
			//This loop is for finding which pid corresponds to which thread. The number of the pid in the index determines where its going to start looking in the array.
		   	for (i = 0; i < numProcs; i++) {
				if (cpid == procs[i]) multiplier = i; 			
			}

			//Now that we know where this child will be starting, start searching.
			int start = MAX_PROCESS_SIZE * multiplier, j;
			for (j = start; j < start + MAX_PROCESS_SIZE; j++) {
				if (arr[j] == target) {
					printf("The target has been found by process %d", cpid);
					exit(1); 
				}						
			}
			exit(0);
		}
	}	
	
	//Wait on the children to finis
	int status, found = 0;
	for (i = 0; i < numProcs; i++) { 
		if (found == 0) { //If we haven't found the target yet, wait.
			waitpid(procs[i], &status, 0);
			if (WIFEXITED(status)) {
				int exit_status = WEXITSTATUS(status);
				if (exit_status == 1) found = 1;
			} 
		} else { //Found the target, time to go on a filicidal killing spree on these processes.
			kill(procs[i], SIGKILL);
		}
	}
	return found; 
}
