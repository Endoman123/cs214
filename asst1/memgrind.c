#include <stdlib.h>
#include <stdio.h>

#include "mymalloc.h"

int main(int argc, char **argv) {
    //Setup for measuring execution time. 
    int workloadA[100]; 
    int workloadB[100];
    int workloadC[100];
    int workloadD[100];
    int workloadE[100];
    int workloadF[100];
    struct timeval start, end;

    //A: malloc() 1 byte and immediately free it - do this 150 times
    int i; for (i = 0; i < 150; i++) {
        gettimeofday(&start, NULL);
        int j; for (j = 0; j < 150; j++) free(malloc(1));
        gettimeofday(&end, NULL);
        workloadA[i] = (int) (end.tv_usec - start.tv_usec);
    }


}