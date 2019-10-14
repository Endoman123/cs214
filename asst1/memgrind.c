#include <stdlib.h>
#include <stdio.h>

#include "mymalloc.h"

double getAverage(int*, int);

int main(int argc, char **argv) {
    // Setup for measuring execution time. 
    int workload[100];
        
    struct timeval start, end;

    // Begin by seeding the random generator
    srand(time(0));
    
    // A: malloc() 1 byte and immediately free it - do this 150 times
    printf("Workload A: malloc() 1 byte and free() x150\n");
    int i; for (i = 0; i < 100; i++) {
        gettimeofday(&start, NULL);
        
        int j; 
        for (j = 0; j < 150; j++) 
            free(malloc(1));
        
        gettimeofday(&end, NULL);
        
        workload[i] = (int) (end.tv_usec - start.tv_usec);
    }
    printf("Average time: %f microseconds\n\n", getAverage(workload, 100));


    // B: malloc() 1 byte, store the pointer in an array - do this 150 times.
    printf("Workload B: malloc() 1 byte and store pointer in array x150\n");
    int j, k; 
    for (i = 0; i < 100; i++) {
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
        gettimeofday(&start, NULL);

        char *buffer[50];   
        int mallocOperations = 0;
        j = -1;

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
    
    return 0;
}

double getAverage(int *values, int nValues) {
    double ret = 0;
    int i = 0;

    for (; i < nValues; i++) {
        ret += values[i];
    }

    return ret / nValues;
}
