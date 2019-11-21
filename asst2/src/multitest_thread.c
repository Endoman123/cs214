#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

const int MAX_THREADS_SIZE = 250;
const char* SEARCH_TYPE = "thread";

int search(int arr[], int arrLen, int target) {
	int numThreads = ceil ((double) arrLen / MAX_THREADS_SIZE);
    //pthreads_t threads[numThreads];
 
    int i;
    pthread_t thread; 
    
    for (i = 0; i < numThreads; i++) {
        //Find where this thread should start.
        int start = MAX_THREAD_SIZE * i;
        int len = MAX_THREAD_SIZE <= arrLen - start ? MAX_THREAD_SIZE : arrLen - start; 
           
        //Make the arguments for the thread since it can only take in a void*. We need to pass in three arguments so lets put it in a struct to make it simple.         
        SearchArgs args = malloc(sizeof(SearchArgs));
        args.arr = arr + start;
        args.arrLen = len;
        args.target = target;
        
        //Make the thread and get its return value from the status
        void* status;
        pthread_create(&thread, NULL, sequentialSearch, args);
        pthread_join(thread, &status);
        int idx = *((int*) status);
        if (idx >= 0) return idx;
    }
    return -1; 

}

typedef struct _SearchArgs {
    int arr[];
    int arrLen;
    int target;
} SearchArgs;

void sequentialSearch(void* args) {


}


