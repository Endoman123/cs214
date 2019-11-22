#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

const char* SEARCH_TYPE = "thread";

typedef struct _SearchArgs {
    int arrLen;
    int target;
    int* resultIdx;
    int* arr;
} SearchArgs;

void* sequentialSearch(void*);

int search(int arr[], int arrLen, int target, int maxSize) {
	int numThreads = ceil ((double) arrLen / maxSize);
    SearchArgs* argsArr[numThreads];
 
    int i;
    pthread_t threads[numThreads]; 
    
    for (i = 0; i < numThreads; i++) {
        pthread_t thread;
        //Find where this thread should start.
        int start = maxSize * i;
        int len = maxSize <= arrLen - start ? maxSize : arrLen - start; 
           
        //Make the arguments for the thread since it can only take in a void*. We need to pass in three arguments so lets put it in a struct to make it simple.         
        SearchArgs* args = malloc(sizeof(SearchArgs*) + sizeof(int) * arrLen);
        args -> arrLen = len;
        args -> target = target;
        args -> resultIdx = malloc(sizeof(int));
        args -> arr = arr + start;
        argsArr[i] = args;
        //Make the thread and get its return value from the status
        pthread_create(&thread, NULL, sequentialSearch, args);
        threads[i] = thread;
    }
   
    int idx = -1; 
    void* status; 
    for (i = 0; i < numThreads; i++) {
        pthread_join(threads[i], &status);
        if (idx == -1) idx = *((int*) status) + maxSize * i;
        free(argsArr[i]);
    }
       
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


