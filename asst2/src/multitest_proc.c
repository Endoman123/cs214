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
		} else if (pid < 0) {
			printf("Error: An error occurred while creating a new process.");
		} else { 
			//The child will get a pid of 0, so it will go to this if. Let's have it search. 
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
					exit(j); 
				}						
			}
			exit(255);
		}
	}	
	
	//Wait on the children to finis
	int status, index = -1;
	for (i = 0; i < numProcs; i++) { 
		if (index == -1) { //If we haven't found the target yet, wait.
			waitpid(procs[i], &status, 0);
			if (WIFEXITED(status)) { //Check if the process ended normally.
				int exit_status = WEXITSTATUS(status); //Get the exit status by grabbing the last 8 bits of the status. 
				if (exit_status != 255) index = exit_status + (MAX_PROCESS_SIZE * i); //The exit status will return a number between 0 and 249. Add it to where the child started its loop.
			} 
		} else { //Found the target, time to go on a filicidal killing spree on these processes.
			kill(procs[i], SIGKILL);
		}
	}
	return index; 
}
