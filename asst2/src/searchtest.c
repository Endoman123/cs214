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

const int SEARCH_ITERATIONS_A = 100;
const int NUM_ELEM_A = 500;
 
const unsigned int WORKLOAD_ITERATIONS = 100, SECONDS_TO_MICROSECONDS = 1000000;


int main(int argc, char** argv) {
    srand(time(0));

    if (argc != 1) {
        printf("Error: Please run with no arguments.\n");
        return -1;
    } 
    
    int i, *arr;
    struct timeval start, end;

    //File setup
    char* filename = malloc(sizeof(char) * 16); //16 is the max length of the filename.
    sprintf(filename, "%s_data.csv", SEARCH_TYPE);
    FILE* fp = fopen(filename, "a+");
    
    //Set up the file header so boomer brains can read it
    //fprintf(fp, "Number of elements, time...\n");
    fprintf(fp, "%d, ", NUM_ELEM_A); //Put the number of elements as the first thing in the row.
    fflush(fp);

    // Initialize array
    arr = calloc(NUM_ELEM_A, sizeof(int));
    for (i = 0; i < NUM_ELEM_A; i++)
        arr[i] = i;  
    
    // Shuffle 
    for (i = 0; i < NUM_ELEM_A; i++) {
        int to = getRandomValue(0, NUM_ELEM_A - 1), temp;

        temp = arr[i];
        arr[i] = arr[to];
        arr[to] = temp;
    }
   
    // Search time
    // We need to keep track of the times elapsed for calculation of the mean, median, and standard deviation
    printf("The search is being conducted with multi-mode -%s\n", SEARCH_TYPE);
    int timevalues_A[SEARCH_ITERATIONS_A];
    
    int val = getRandomValue(0, NUM_ELEM_A - 1);
    
    printf("Test A\n");
    printf("Run the search %d times with an array of size %d\n", SEARCH_ITERATIONS_A, NUM_ELEM_A);
    for (i = 0; i < SEARCH_ITERATIONS_A; i++) {
        int valIdx, newIdx, temp, time;

        gettimeofday(&start, NULL);
        valIdx = search(arr, NUM_ELEM_A, val);
        gettimeofday(&end, NULL);
        time = (int) ((end.tv_sec * SECONDS_TO_MICROSECONDS + end.tv_usec) - (start.tv_sec * SECONDS_TO_MICROSECONDS + start.tv_usec)); 
        timevalues_A[i] = time;

        //Swap the target with a random index for our next search.
        newIdx = getRandomValue(0, NUM_ELEM_A - 1);
        temp = arr[valIdx];
        arr[valIdx] = arr[newIdx];
        arr[newIdx] = temp;
    } 
    
    double min = getMin(timevalues_A, SEARCH_ITERATIONS_A);
    double max = getMax(timevalues_A, SEARCH_ITERATIONS_A);
    double mean = getMean(timevalues_A, SEARCH_ITERATIONS_A);    
    double stdev = getStandardDeviation(timevalues_A, SEARCH_ITERATIONS_A);
    
    printf("Min: %d\n", min);
    printf("Max: %d\n", max); 
    printf("Mean: %d\n", mean);
    printf("Standard Devation: %d\n", stdev);

    fclose(fp);
    return 0;
}

int getRandomValue(int min, int max) {
    return (int)((float) rand() / RAND_MAX * (max - min + 1) + min);
}

int seqSearch(int arr[], int arrLen, int target) {
    int i;
    for (i = 0; i < arrLen; i++) if (arr[i] == target) return i;   
    return -1;
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
