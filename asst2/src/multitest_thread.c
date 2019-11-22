#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

const int MAX_THREAD_SIZE = 250;
const char* SEARCH_TYPE = "thread";

typedef struct _SearchArgs {
    int arrLen;
    int target;
    int* resultIdx;
    int* arr;
} SearchArgs;

void* sequentialSearch(void*);

int search(int arr[], int arrLen, int target) {
	int numThreads = ceil ((double) arrLen / MAX_THREAD_SIZE);
 
    int i;
    pthread_t threads[numThreads]; 
    
    for (i = 0; i < numThreads; i++) {
        pthread_t thread;
        //Find where this thread should start.
        int start = MAX_THREAD_SIZE * i;
        int len = MAX_THREAD_SIZE <= arrLen - start ? MAX_THREAD_SIZE : arrLen - start; 
           
        //Make the arguments for the thread since it can only take in a void*. We need to pass in three arguments so lets put it in a struct to make it simple.         
        SearchArgs* args = malloc(sizeof(SearchArgs*) + sizeof(int) * arrLen);
        args -> arrLen = len;
        args -> target = target;
        args -> resultIdx = malloc(sizeof(int));
        args -> arr = arr + start;

        //Make the thread and get its return value from the status
        pthread_create(&thread, NULL, sequentialSearch, args);
        threads[i] = thread;
    }
   
    int idx = -1; 
    void* status; 
    for (i = 0; i < numThreads; i++) {
        pthread_join(threads[i], &status);
        if (idx == -1) idx = *((int*) status) + MAX_THREAD_SIZE * i;
        printf("Thread found index %d\n", idx);
    }
       
    printf("End search\n\n");
    if (idx >= 0) return idx;
    else return -1;
}


void* sequentialSearch(void* sArgs) {
    SearchArgs* args = (SearchArgs*) sArgs;
    int i;
    *(args -> resultIdx) = -1;
    for (i = 0; i < args -> arrLen; i++)
         if (args -> arr[i] == args -> target) {
            //Return the int as a pointer. Since it's bad practice to return the address of a local variable, lets use another part of the struct to return the index.
            *(args -> resultIdx) = i;
            break;
        }
    pthread_exit(args -> resultIdx); 
}


