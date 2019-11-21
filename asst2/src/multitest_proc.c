#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <math.h>

const int MAX_PROCESS_SIZE = 250;
const char* SEARCH_TYPE = "process";

int search_proc(int[], int, int, int);

int search(int arr[], int length, int target) {
    return search_proc(arr, 0, length, target);
}

int search_proc(int arr[], int from, int length, int target) {
    int i = from;
    for (; i - from < length; i++) {
        if (arr[i] == target)
            return i;
    }    

	return -1; 
}
