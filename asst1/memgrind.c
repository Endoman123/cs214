#include <stdlib.h>
#include <stdio.h>

#include "mymalloc.h"

int main(int argc, char **argv) {
    // Setup for measuring execution time. 
    int workloadA[100]; 
    int workloadB[100];
    int workloadC[100];
    int workloadD[100];
    int workloadE[100];
    int workloadF[100];
    
    struct timeval start, end;

    // Begin by seeding the random generator
    srand(time(0));
    
    // A: malloc() 1 byte and immediately free it - do this 150 times
    int i; for (i = 0; i < 100; i++) {
        gettimeofday(&start, NULL);
        int j; for (j = 0; j < 150; j++) free(malloc(1));
        gettimeofday(&end, NULL);
        workloadA[i] = (int) (end.tv_usec - start.tv_usec);
    }

    // B: malloc() 1 byte, store the pointer in an array - do this 150 times.
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
        workloadB[i] = (int) (end.tv_usec - start.tv_usec);
    }

    // C: Randomly choose between a 1 byte malloc() or free()ing a 1 byte pointer
    for (i = 0; i < 100; i++) {
        gettimeofday(&start, NULL);

        char *buffer[50];   
        int mallocOperations = 0;
        j = 0;

        // Generate a random number. If even -> malloc. If odd -> free.
        // Stop after mallocing 50 times.
        while (mallocOperations < 50) {
            if (rand() % 2 == 0) 
                buffer[j++] = malloc(1);
            else free(buffer[j--]);
        }
        for (k = 0; k <= j; k++) free(buffer[k]);

        gettimeofday(&end, NULL);

        workloadC[i] = (int) (end.tv_usec - start.tv_usec);

    }


    return 0;
}
