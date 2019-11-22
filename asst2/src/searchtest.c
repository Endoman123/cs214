#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "multitest.h"

int getRandomValue(int, int);
int seqSearch(int[], int, int);
double getMax(int[], int);
double getMin(int[], int);
double getMean(int[], int);
double getStandardDeviation(int[], int);
int *performWorkload(int, int, int *);
 
const unsigned int WORKLOAD_ITERATIONS = 100, SECONDS_TO_MICROSECONDS = 1000000;

int main(int argc, char** argv) {
    srand(time(0));

    if (argc != 1) {
        printf("Error: Please run with no arguments.\n");
        return -1;
    } 
    
    

    //File setup
    char* filename = malloc(sizeof(char) * 16); //16 is the max length of the filename.
    sprintf(filename, "%s_data.csv", SEARCH_TYPE);
    FILE* fp = fopen(filename, "a+");

    // Search time
    // We need to keep track of the times elapsed for calculation of the mean, median, and standard deviation
    printf("The search is being conducted with multi-mode -%s\n", SEARCH_TYPE);
    int i, *timevalues = calloc(WORKLOAD_ITERATIONS, sizeof(int));
    double min, max, mean, stdev;
    
    // for (i = 1000; i < 1000000; i += 1000) { 
        // printf("Test A: %d elements, 100 elements per coprocess\n", i);
        timevalues = performWorkload(100, 10, timevalues);
    
        min = getMin(timevalues, WORKLOAD_ITERATIONS);
        max = getMax(timevalues, WORKLOAD_ITERATIONS);
        mean = getMean(timevalues, WORKLOAD_ITERATIONS);    
        stdev = getStandardDeviation(timevalues, WORKLOAD_ITERATIONS);
        
        printf("Min: %d\n", min);
        printf("Max: %d\n", max); 
        printf("Mean: %d\n", mean);
        printf("Standard Devation: %d\n\n", stdev);
    // }

    fclose(fp);
    return 0;
}

int getRandomValue(int min, int max) {
    return (int)((float) rand() / RAND_MAX * (max - min + 1) + min);
}

double getMin(int arr[], int arrLen) {
    int i; 
    double min = -1;
    for (i = 0; i < arrLen; i++) if (arr[i] < min) min = arr[i];
    return min;
}

double getMax(int arr[], int arrLen) {
    int i;
    double max = arr[0];
    for (i = 1; i < arrLen; i++) if (arr[i] > max) max = arr[i];
    return max;
}

double getMean(int arr[], int arrLen) {
    int i, mean = 0;
    for (i = 0; i < arrLen; i++) mean += arr[i];
    return mean / arrLen;
}

double getStandardDeviation(int arr[], int arrLen) {
    //Step 1: Get the mean
    int mean = getMean(arr, arrLen);
    
    //Step 2: Subtract the mean from every element in the array and square it
    int i;
    int squareDiffs[arrLen];
    for (i = 0; i < arrLen; i++) squareDiffs[i] = pow(arr[i] - mean, 2);
    
    //Step 3: Get the mean of the differences
    int diffMean = getMean(squareDiffs, arrLen);
    return sqrt(diffMean);
}

/**
 * Performs a workload given the specified array length and max array chunk size per process/thread.
 *
 * Returns array of time values for each iteration
 */
int *performWorkload(int arrLen, int maxSize, int *timevalues) {
    int i, *arr;
    struct timeval start, end;
    
    // Initialize array
    arr = calloc(arrLen, sizeof(int));
    for (i = 0; i < arrLen; i++)
        arr[i] = i;  
    
    // Shuffle 
    for (i = 0; i < arrLen; i++) {
        int to = getRandomValue(0, arrLen - 1), temp;

        temp = arr[i];
        arr[i] = arr[to];
        arr[to] = temp;
    }

    // Get random value to search for
    int val = getRandomValue(0, arrLen - 1); 
    for (i = 0; i < WORKLOAD_ITERATIONS; i++) {
       int valIdx, newIdx, temp, time;

       // Time
       gettimeofday(&start, NULL);
       valIdx = search(arr, arrLen, val, maxSize);
       gettimeofday(&end, NULL);
       time = (int) ((end.tv_sec * SECONDS_TO_MICROSECONDS + end.tv_usec) - (start.tv_sec * SECONDS_TO_MICROSECONDS + start.tv_usec)); 
       timevalues[i] = time;
       
       // Swap the target with a random index for our next search.
       newIdx = getRandomValue(0, arrLen - 1);
       temp = arr[valIdx];
       arr[valIdx] = arr[newIdx];
       arr[newIdx] = temp;
    }

    // Free the array before returning
    // free(arr);

    return timevalues;
}
