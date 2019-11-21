#include <stdlib.h>
#include <stdio.h>

#include "multitest.h"

int getRandomValue(int, int);

const unsigned int WORKLOAD_ITERATIONS = 100, SECONDS_TO_MICROSECONDS = 1000000;

int main(int argc, char** argv) {
    srand(time(0));

    if (argc != 2) {
        printf("Error: Incorrect number of arguments.\n");
        return -1;
    } 
    
    int nElem, i, *arr;
    struct timeval start, end;

    sscanf(argv[1], "%d", &nElem);

    // Initialize array
    arr = calloc(nElem, sizeof(int));
    for (i = 0; i < nElem; ++i)
        arr[i] = i;

    // Shuffle 
    for (i = 0; i < nElem; ++i) {
        int to = getRandomValue(0, nElem - 1), temp;

        temp = arr[i];
        arr[i] = arr[to];
        arr[to] = arr[i];
    }
    
    // Search time
    // Ihadurca here, time to cloud the server with too many tests
    int val = getRandomValue(0, nElem - 1);
    for (i = 0; i < 10; ++i) {
        int from, to, temp, time;

        gettimeofday(&start, NULL);
        from = search(arr, nElem, val);
        gettimeofday(&end, NULL);
        time = (int) ((end.tv_sec * SECONDS_TO_MICROSECONDS + end.tv_usec) - (start.tv_sec * SECONDS_TO_MICROSECONDS + start.tv_usec)); 
        
        if (arr[from] == val)
            printf("found\n");
        
        to = getRandomValue(0, nElem - 1);
        
        temp = arr[from];
        arr[from] = arr[to];
        arr[to] = temp;
    }
    
    return 0;
}

int getRandomValue(int min, int max) {
    return (int)((float) rand() / RAND_MAX * (max - min + 1) + min);
}
