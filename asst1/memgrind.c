#include <stdlib.h>
#include <stdio.h>

#include "mymalloc.h"

double getAverage(int*, int);
int getRandomNumber(int, int);
typedef enum boolean {true = 1, false = 0} boolean;
boolean flipCoin();

int main(int argc, char **argv) {
    //Constants
    const unsigned int WORKLOAD_ITERATIONS = 100;
    const unsigned short BLOCK_SIZE = 4096;
    const unsigned short METADATA_SIZE = 2;  
    const unsigned int SECONDS_TO_MICROSECONDS = 1000000;
    
    // Setup for measuring execution time. 
    int i, j, k, workload[WORKLOAD_ITERATIONS];
        
    struct timeval start, end;

    // Begin by seeding the random generator
    srand(time(0));
   
    // A: malloc() 1 byte and immediately free it - do this 150 times
    printf("Workload A: malloc 1 byte and free x150\n");

    //Workload Constants
    const unsigned short CYCLE_SIZE_A = 150;
    const unsigned short MALLOC_SIZE_A = 1;

    for (i = 0; i < WORKLOAD_ITERATIONS; i++) {        
        gettimeofday(&start, NULL);
        
        for (j = 0; j < CYCLE_SIZE_A; j++) 
            free(malloc(MALLOC_SIZE_A));
        
        gettimeofday(&end, NULL);
        
        workload[i] = (int) ((end.tv_sec * SECONDS_TO_MICROSECONDS + end.tv_usec) - (start.tv_sec * SECONDS_TO_MICROSECONDS + start.tv_usec));
    }
    printf("Average time: %f microseconds\n\n", getAverage(workload, WORKLOAD_ITERATIONS));


    // B: malloc() 1 byte, store the pointer in an array - do this 150 times.
    printf("Workload B: malloc 1 byte and store pointer in array x150\n"); 

    //Workload constants
    const unsigned short CYCLE_SIZE_B = 50;
    const unsigned short NUM_CYCLES_B = 3;
    const unsigned short MALLOC_SIZE_B = 1;

    for (i = 0; i < WORKLOAD_ITERATIONS; i++) {
        gettimeofday(&start, NULL);
        
        for (j = 0; j < NUM_CYCLES_B; j++) { //Repeat the 50 malloc 50 free cycle 3 times for 150.
            char *buffer[CYCLE_SIZE_B];
            for (k = 0; k < CYCLE_SIZE_B; k++)
                buffer[k] = malloc(MALLOC_SIZE_B);

            for (k = 0; k < CYCLE_SIZE_B; k++)
                free(buffer[k]);
        }

        gettimeofday(&end, NULL);

        workload[i] = (int) ((end.tv_sec * SECONDS_TO_MICROSECONDS + end.tv_usec) - (start.tv_sec * SECONDS_TO_MICROSECONDS + start.tv_usec));
    }
    printf("Average time: %f microseconds\n\n", getAverage(workload, WORKLOAD_ITERATIONS));

    // C: Randomly choose between a 1 byte malloc() or free()ing a 1 byte pointer
    printf("Workload C: Random 1 byte malloc and free, up to 50 mallocs\n");

    //Workload Constants
    const unsigned short MAX_MALLOCS_C = 50; //Maximum of 50 mallocs per workload.
    
    for (i = 0; i < WORKLOAD_ITERATIONS; i++) {
        char *buffer[MAX_MALLOCS_C];   
        int mallocOperations = 0, j = -1, k = 0;
        
        gettimeofday(&start, NULL);

        // Generate a random number. If even -> malloc. If odd -> free.
        // Stop after mallocing 50 times. Do not free when there are no pointers to.
        while (mallocOperations < MAX_MALLOCS_C) {
            if (flipCoin()) { 
                buffer[++j] = malloc(1);
                mallocOperations++;
            } else if (j > 0)
                free(buffer[j--]);
        }
       
        // Free everything
        for (k = 0; k <= j; k++) 
            free(buffer[k]);

        gettimeofday(&end, NULL);

        workload[i] = (int) ((end.tv_sec * SECONDS_TO_MICROSECONDS + end.tv_usec) - (start.tv_sec * SECONDS_TO_MICROSECONDS + start.tv_usec));
    }
    printf("Average time: %f microseconds\n\n", getAverage(workload, WORKLOAD_ITERATIONS));
    
    // D: Randomly choose between a randomly-sized malloc and a free 150 times.
    printf("Workload D: Random malloc and free, up to 50 mallocs\n");

    //Workload Constants
    const unsigned short MAX_MALLOCS_D = 50;
    const unsigned short MIN_ALLOC = 1;
    const unsigned short MAX_ALLOC = 64;

    for (i = 0; i < WORKLOAD_ITERATIONS; i++) {
        char *buffer[MAX_MALLOCS_D];   
        int mallocOperations = 0, remainingMemory = BLOCK_SIZE, j = -1, k = 0;
        
        gettimeofday(&start, NULL);

        // Generate a random number. If even -> malloc. If odd -> free.
        // Stop after mallocing 50 times. Do not free when there are no pointers to.
        while (mallocOperations < MAX_MALLOCS_D) {
            if (flipCoin()) {
                unsigned short size = (unsigned short) getRandomNumber(MIN_ALLOC, MAX_ALLOC); 

                // Check to make sure we have enough memory to allocate
                // a pointer
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

        gettimeofday(&end, NULL);

        workload[i] = (int) ((end.tv_sec * SECONDS_TO_MICROSECONDS + end.tv_usec) - (start.tv_sec * SECONDS_TO_MICROSECONDS + start.tv_usec));
    }
    printf("Average time: %f microseconds\n\n", getAverage(workload, WORKLOAD_ITERATIONS));

    // E: Populate static block, remove middle, attempt to fill completely 
    printf("Workload E: Populate static block, remove middle, attempt to fill completely\n");
    for (i = 0; i < WORKLOAD_ITERATIONS; i++) {
        unsigned short *buffer[17]; 
        gettimeofday(&start, NULL);

        // Step 1: Create blocks
        for (j = 0; j < 16; j++) {
            buffer[j] = malloc(254);
        }

        // Step 2: Remove plot in the middle
        int plotstart = getRandomNumber(0, 7);
        int plotend = getRandomNumber(8, 16);

        for (j = plotstart; j < plotend; j++) {
            free(buffer[j]);
            buffer[j] = NULL;
        }

        // Step 3: Attempt to allocate the middle with 1 large block
        buffer[16] = malloc((plotend - plotstart) * 256 - 2);

        // Free everything afterwards 
        for (k = 0; k < 17; k++) { 
            if (buffer[k] != NULL)
                free(buffer[k]);
        }

        gettimeofday(&end, NULL);

        workload[i] = (int) ((end.tv_sec * SECONDS_TO_MICROSECONDS + end.tv_usec) - (start.tv_sec * SECONDS_TO_MICROSECONDS + start.tv_usec));
    }
    printf("Average time: %f microseconds\n\n", getAverage(workload, WORKLOAD_ITERATIONS));

    // F: Randomly allocate, randomly free, and randomly reallocate.
    printf("Workload F: Random allocations, frees, and reallocations\n");
    const short ALLOCATION_THRESHOLD = 1024, MAX_ALLOCATION = 128;
    for (i = 0; i < WORKLOAD_ITERATIONS; i++) {
        gettimeofday(&start, NULL);
        int remainingMemory = BLOCK_SIZE; j = 0;
        unsigned short size, *buffer[128];

        //Allocate around only about 3/4ths of the block
        //so there is room for variance without segfaulting.  
        while (remainingMemory > ALLOCATION_THRESHOLD) {
            //Populate the buffer with random allocations
            size = getRandomNumber(1, MAX_ALLOCATION) * sizeof(unsigned short); //Get a random amount of memory for shorts.
            buffer[j] = malloc(size);
            *buffer[j++] = size;
            remainingMemory = remainingMemory - (size + METADATA_SIZE); 
        }
        
        // Repeatedly and randomly free and reallocate until half of the block is available.
        // 50% chance to free a given pointer
        // 25% chance to reallocate a pointer. 
        while (remainingMemory < 2048) {
            for (k = 0; k < j; k++) {
                if (flipCoin()) { 
                    //50% chance to free the pointer.
                    if (buffer[k] != NULL) {
                        remainingMemory += *buffer[k] + METADATA_SIZE;
                        free(buffer[k]);
                        buffer[k] = NULL;
                    }
                    if (flipCoin()) { 
                        //50% to reallocate from the pointer that was just freed. 
                        //Smaller range to prevent segfaults due to block splitting.
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

        gettimeofday(&end, NULL);
        workload[i] = (int) ((end.tv_sec * SECONDS_TO_MICROSECONDS + end.tv_usec) - (start.tv_sec * SECONDS_TO_MICROSECONDS + start.tv_usec));
    }
    printf("Average time: %f microseconds\n\n", getAverage(workload, WORKLOAD_ITERATIONS));

    return 0;
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
    return (int)((float)rand() / RAND_MAX * (max - min + 1) + min);
}

/**
 * Generate a true or false randomly. 
*/ 
boolean flipCoin() {
    return (boolean) rand() % 2;
}