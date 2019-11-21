#include <stdlib.h>
#include <stdio.h>

#include "multitest.h"

int getRandomValue(int, int);
const int SEARCH_ITERATIONS = 10;

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
    
    //File setup
    char* filename = malloc(sizeof(char) * 16); //16 is the max length of the filename.
    sprintf(filename, "%s_data.csv", SEARCH_TYPE);
    FILE* fp = fopen(filename, "a+");
    
    //Set up the file header so boomer brains can read it
    //fprintf(fp, "Number of elements, time...\n");
    fprintf(fp, "%d, ", nElem); //Put the number of elements as the first thing in the row.
    fflush(fp);
    // Initialize array
    arr = calloc(nElem, sizeof(int));
    for (i = 0; i < nElem; i++)
        arr[i] = i;
    
    
    // Shuffle 
    for (i = 0; i < nElem; i++) {
        int to = getRandomValue(0, nElem - 1), temp;

        temp = arr[i];
        arr[i] = arr[to];
        arr[to] = temp;
    }
   
    // Search time
    // Ihadurca here, time to cloud the server with too many tests
    int val = getRandomValue(0, nElem - 1);
    for (i = 0; i < SEARCH_ITERATIONS; i++) {
        int from, to, temp, time;

        gettimeofday(&start, NULL);
        from = search(arr, nElem, val);
        gettimeofday(&end, NULL);
        time = (int) ((end.tv_sec * SECONDS_TO_MICROSECONDS + end.tv_usec) - (start.tv_sec * SECONDS_TO_MICROSECONDS + start.tv_usec)); 
        if (i < SEARCH_ITERATIONS - 1) fprintf(fp, "%d, ", time); //Each number after the total number of elements is the time it took to search.
        else fprintf(fp, "%d", time); //To make the rows look cleaner, lets end the row without a comma.

        fflush(fp);
        //Swap the target with a random index for our next search.
        to = getRandomValue(0, nElem - 1);
        temp = arr[from];
        arr[from] = arr[to];
        arr[to] = temp;
    }

    fclose(fp);    
    return 0;
}

int getRandomValue(int min, int max) {
    return (int)((float) rand() / RAND_MAX * (max - min + 1) + min);
}
