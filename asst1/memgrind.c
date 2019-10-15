#include <stdlib.h>
#include <stdio.h>

#include "mymalloc.h"

double getAverage(int*, int);
int getRandomNumber(int, int);

int main(int argc, char **argv) {
    // Setup for measuring execution time. 
    int workload[100], i;
        
    struct timeval start, end;

    // Begin by seeding the random generator
    srand(time(0));
   
    // A: malloc() 1 byte and immediately free it - do this 150 times
    printf("Workload A: malloc 1 byte and free x150\n");
    for (i = 0; i < 100; i++) {
        int j = 0;
        
        gettimeofday(&start, NULL);
        
        for (j = 0; j < 150; j++) 
            free(malloc(1));
        
        gettimeofday(&end, NULL);
        
        workload[i] = (int) (end.tv_usec - start.tv_usec);
    }
    printf("Average time: %f microseconds\n\n", getAverage(workload, 100));


    // B: malloc() 1 byte, store the pointer in an array - do this 150 times.
    printf("Workload B: malloc 1 byte and store pointer in array x150\n"); 
    for (i = 0; i < 100; i++) {
        int j, k;

        gettimeofday(&start, NULL);
        
        for (j = 0; j < 3; j++) { //Repeat the 50 malloc 50 free cycle 3 times for 150.
            char *buffer[50];
            for (k = 0; k < 50; k++)
                buffer[k] = malloc(1);

            for (k = 0; k < 50; k++)
                free(buffer[k]);
        }

        gettimeofday(&end, NULL);

        workload[i] = (int) (end.tv_usec - start.tv_usec);
    }
    printf("Average time: %f microseconds\n\n", getAverage(workload, 100));

    // C: Randomly choose between a 1 byte malloc() or free()ing a 1 byte pointer
    printf("Workload C: Random 1 byte malloc and free, up to 50 mallocs\n");
    for (i = 0; i < 100; i++) {
        char *buffer[50];   
        int mallocOperations = 0, j = -1, k = 0;
        
        gettimeofday(&start, NULL);

        // Generate a random number. If even -> malloc. If odd -> free.
        // Stop after mallocing 50 times. Do not free when there are no pointers to.
        while (mallocOperations < 50) {
            if (rand() % 2 == 0) { 
                buffer[++j] = malloc(1);
                mallocOperations++;
            } else if (j > 0)
                free(buffer[j--]);
        }
       
        // Free everything after 50 malloc operations 
        for (k = 0; k <= j; k++) 
            free(buffer[k]);

        gettimeofday(&end, NULL);

        workload[i] = (int) (end.tv_usec - start.tv_usec);
    }
    printf("Average time: %f microseconds\n\n", getAverage(workload, 100));
    
    // D: Randomly choose between a randomly-sized malloc and a free 150 times.
    printf("Workload D: Random malloc and free, up to 50 mallocs\n");
    for (i = 0; i < 100; i++) {
        char *buffer[50];   
        int mallocOperations = 0, remainingMemory = 4096, j = -1, k = 0;
        
        gettimeofday(&start, NULL);

        // Generate a random number. If even -> malloc. If odd -> free.
        // Stop after mallocing 50 times. Do not free when there are no pointers to.
        while (mallocOperations < 50) {
            if (rand() % 2 == 0) {
                unsigned short size = (unsigned short)getRandomNumber(1, 64); 

                // Assuming that our metadata is 2 bytes,
                // check to make sure we have enough memory to allocate
                // a pointer
                if (remainingMemory - size - 2 > 0) {
                    remainingMemory -= size + 2; 
                    buffer[++j] = malloc(size);
                    *buffer[j] = (char)size;
                    mallocOperations++;
                }
            } else if (j > 0) {
                remainingMemory += 2 + (int)*buffer[j];
                free(buffer[j--]);
            }
        }
       
        // Free everything after 50 malloc operations 
        for (k = 0; k <= j; k++) 
            free(buffer[k]);

        gettimeofday(&end, NULL);

        workload[i] = (int) (end.tv_usec - start.tv_usec);
    }
    printf("Average time: %f microseconds\n\n", getAverage(workload, 100));

    // E: Populate static block, remove middle, attempt to fill completely 
    printf("Workload E: Populate static block, remove middle, attempt to fill completely\n");
    for (i = 0; i < 100; i++) {
        unsigned short *buffer[16]; 
        int j = 0;
        
        gettimeofday(&start, NULL);

        // Step 1: Create blocks
        for (j = 0; j < 16; j++) {
            buffer[j] = malloc(254);
        }

        // Step 2: Remove plot in the middle
        for (j = 1; j < 15; j++) {
            free(buffer[j]);
            buffer[j] = NULL;
        }

        // Step 3: Attempt to allocate the middle with 1 large block
        buffer[1] = malloc(3582);

        // Free everything afterwards 
        int k = 0;
        for (k = 0; k < 16; k++) { 
            if (buffer[k] != NULL)
                free(buffer[k]);
        }

        gettimeofday(&end, NULL);

        workload[i] = (int) (end.tv_usec - start.tv_usec);
    }
    
    printf("Average time: %f microseconds\n\n", getAverage(workload, 100));

    return 0;
}

/**
 * Gets the average of a set of values
 */
double getAverage(int *values, int nValues) {
    double ret = 0;
    int i = 0;

    for (; i < nValues; i++) {
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
