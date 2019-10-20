#include <stdlib.h>
#include <stdio.h>

#include "mymalloc.h"

double getAverage(int*, int);
int getRandomNumber(int, int);

void runWorkloadA();
void runWorkloadB();
void runWorkloadC();
void runWorkloadD();
void runWorkloadE();
void runWorkloadF();

const unsigned short BLOCK_SIZE = 4096, METADATA_SIZE = 2;  

int main(int argc, char* argv[]) {
    // Constants and the workload function array.
    const unsigned int WORKLOAD_ITERATIONS = 100, SECONDS_TO_MICROSECONDS = 1000000;

    void (*workloads[6]) () = {&runWorkloadA, &runWorkloadB, &runWorkloadC, &runWorkloadD, &runWorkloadE, &runWorkloadF};
    char* workloadDescriptions[6] = {
        "Workload A: malloc 1 byte and free x150\n",
        "Workload B: malloc 1 byte and store pointer in array x150\n",
        "Workload C: Random 1 byte malloc and free, up to 50 mallocs\n",
        "Workload D: Random malloc and free, up to 50 mallocs\n",
        "Workload E: Allocate all available memory into blocks, free randomly sized section, attempt to fill hole completely\n",
        "Workload F: Random allocations, frees, and reallocations\n"
    };

    // For time recording purposes.
    struct timeval start, end;
    int workloadTimes[WORKLOAD_ITERATIONS];

    // Seed the random number generator for the workloads that do random generations.
    srand(time(0));

    // Run each workload and record the times, printing an average for each.
    int i, j; for (i = 0; i < 6; i++) {
        printf(workloadDescriptions[i]);
        for (j = 0; j < WORKLOAD_ITERATIONS; j++) {
            gettimeofday(&start, NULL);
            workloads[i]();
            gettimeofday(&end, NULL);
            workloadTimes[j] = (int) ((end.tv_sec * SECONDS_TO_MICROSECONDS + end.tv_usec) - (start.tv_sec * SECONDS_TO_MICROSECONDS + start.tv_usec));
        }
        printf("Average time: %f microseconds\n\n", getAverage(workloadTimes, WORKLOAD_ITERATIONS));
    }
    
    return 0;
}

/**
 * Workload A - malloc() 1 byte and immediately free it - do this 150 times
 */
void runWorkloadA() {
    // Workload Constants
    const unsigned short CYCLE_SIZE_A = 150, MALLOC_SIZE_A = 1; // Run 150 cycles of mallocing and freeing 1 byte per workload.

    int j; for (j = 0; j < CYCLE_SIZE_A; j++) 
        free(malloc(MALLOC_SIZE_A));
}

/**
 * Workload B - malloc() 1 byte, store the pointer in an array - do this 150 times.
 */
void runWorkloadB() {
    // Workload constants
    const unsigned short CYCLE_SIZE_B = 50, NUM_CYCLES_B = 3, MALLOC_SIZE_B = 1; // Run 3 cycles of 50 frees and 50 mallocs with mallocs of 1 byte per workload.

    int j, k; for (j = 0; j < NUM_CYCLES_B; j++) { // Repeat the 50 malloc 50 free cycle 3 times for 150.
        char *buffer[CYCLE_SIZE_B];
        for (k = 0; k < CYCLE_SIZE_B; k++)
            buffer[k] = malloc(MALLOC_SIZE_B);

        for (k = 0; k < CYCLE_SIZE_B; k++)
            free(buffer[k]);
    }
}

/**
 * Workload C - Randomly choose between a 1 byte malloc() or free()ing a 1 byte pointer
 */
void runWorkloadC() {
    // Workload Constants
    const unsigned short MAX_MALLOCS_C = 50; //Maximum of 50 mallocs per workload.
    
    char *buffer[MAX_MALLOCS_C];   
    int mallocOperations = 0;
    int j = -1, k = 0;
    
    // 50/50 chance to malloc or free.
    // Stop after mallocing 50 times. Do not free when there are no pointers to.
    while (mallocOperations < MAX_MALLOCS_C) {
        if (getRandomNumber(0, 1)) { 
            buffer[++j] = malloc(1);
            mallocOperations++;
        } else if (j > 0)
            free(buffer[j--]);
    }
    // Free everything
    for (k = 0; k <= j; k++) 
        free(buffer[k]);
}

/**
 * Workload D - Randomly choose between a randomly-sized malloc and a free 150 times.
 */ 
void runWorkloadD() {
    // Workload Constants
    const unsigned short MAX_MALLOCS_D = 50, MIN_ALLOC = 1, MAX_ALLOC = 64;

    char *buffer[MAX_MALLOCS_D];   
    int mallocOperations = 0, remainingMemory = BLOCK_SIZE, j = -1, k = 0;

    // 50/50 chance to malloc or free.
    // Stop after mallocing 50 times. Do not free when there are no pointers to.
    while (mallocOperations < MAX_MALLOCS_D) {
        if (getRandomNumber(0, 1)) {
            unsigned short size = (unsigned short) getRandomNumber(MIN_ALLOC, MAX_ALLOC); 

            // Check to make sure we have enough memory to allocate a pointer
            if (remainingMemory - size - METADATA_SIZE > 0) {
                remainingMemory -= size + METADATA_SIZE; 
                buffer[++j] = malloc(size);
                *buffer[j] = (char)size;
                mallocOperations++;
            }
        } else if (j > 0) {
            remainingMemory += METADATA_SIZE + (int)*buffer[j];
            free(buffer[j--]);
        }
    }
    
    // Free everything after 50 malloc operations 
    for (k = 0; k <= j; k++) 
        free(buffer[k]);
}

/**
 * Workload E - Allocate all available memory into blocks, free randomly sized section, attempt to fill hole completely
 */
void runWorkloadE() {
    // Workload constants
    const unsigned short NUM_BLOCKS_E = 16, SIZE_PER_BLOCK_E = BLOCK_SIZE / NUM_BLOCKS_E;
    unsigned short *buffer[NUM_BLOCKS_E + 1]; 

    // Step 1: Create blocks
    int j;
    for (j = 0; j < NUM_BLOCKS_E; j++) {
        buffer[j] = malloc(SIZE_PER_BLOCK_E - METADATA_SIZE);
    }

    // Step 2: Generate randomly sized block to remove. 
    int plotstart = getRandomNumber(0, NUM_BLOCKS_E / 2 - 1);
    int plotend = getRandomNumber(NUM_BLOCKS_E / 2 , NUM_BLOCKS_E);

    for (j = plotstart; j < plotend; j++) {
        free(buffer[j]);
        buffer[j] = NULL;
    }

    // Step 3: Attempt to reallocate the removed blocks with a single large block.
    buffer[NUM_BLOCKS_E] = malloc((plotend - plotstart) * (SIZE_PER_BLOCK_E) - METADATA_SIZE);

    // Free everything afterwards 
    int k; for (k = 0; k <= NUM_BLOCKS_E; k++) { 
        if (buffer[k] != NULL)
            free(buffer[k]);
    }
}

/**
 * Workload F - Randomly allocate, randomly free, and randomly reallocate.
 */
void runWorkloadF() {
    const short ALLOCATION_THRESHOLD = 1024, MAX_ALLOCATION = 128;
    int remainingMemory = BLOCK_SIZE, j = 0, k = 0;
    unsigned short size, *buffer[MAX_ALLOCATION];

    while (remainingMemory > ALLOCATION_THRESHOLD) {
        // Populate the buffer with random allocations
        size = getRandomNumber(1, MAX_ALLOCATION) * sizeof(unsigned short); // Get a random amount of memory for shorts.
        buffer[j] = malloc(size);
        *buffer[j++] = size;
        remainingMemory = remainingMemory - (size + METADATA_SIZE); 
    }
    
    // Repeatedly and randomly free and reallocate until half of the block is available.
    // 50% chance to free a given pointer
    // 25% chance to reallocate a pointer. 
    while (remainingMemory < BLOCK_SIZE / 2) {
        for (k = 0; k < j; k++) {
            if (getRandomNumber(0, 1)) { 
                // 50% chance to free the pointer.
                if (buffer[k] != NULL) {
                    remainingMemory += *buffer[k] + METADATA_SIZE;
                    free(buffer[k]);
                    buffer[k] = NULL;
                }
                if (getRandomNumber(0, 1)) { 
                    // 50% to reallocate from the pointer that was just freed. 
                    // Smaller range to prevent segfaults due to block splitting.
                    size = getRandomNumber(1, MAX_ALLOCATION / 2) * sizeof(unsigned short);
                    buffer[k] = malloc(size);
                    *buffer[k] = size; 
                    remainingMemory -= size + METADATA_SIZE; 
                }
            }
        } 
    }

    // Free all remaining pointers.  
    for (k = 0; k < j; k++) { 
        if (buffer[k] != NULL) {
            free(buffer[k]);
            buffer[k] = NULL;
        }
    }
}

/**
 * Gets the average of a set of values
 */
double getAverage(int *values, int nValues) {
    double ret = 0;
    
    int i; for (i = 0; i < nValues; i++) {
        ret += values[i];
    }

    return ret / nValues;
}

/**
 * Generates a random number between min and max
 */
int getRandomNumber(int min, int max) {
    return (int)((float) rand() / RAND_MAX * (max - min + 1) + min);
}
