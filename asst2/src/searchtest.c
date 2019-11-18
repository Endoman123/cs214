#include <stdlib.h>

#include "multitest.h"

int getRandomValue(int, int);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Use: searchtest [nElem]");
        return -1;
    }

    int nElem, i, *arr;

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
    int val = getRandomValue(0, nElem - 1);
    for (i = 0; i < 100; ++i) {
        int from = search(arr, val), temp;

        to = getRandomValue(0, nElem - 1);
        temp = arr[from];
        arr[from] = arr[to];
        arr[to] = arr[from];
    } 
    
    return 0;
}

int getRandomValue(int min, int max) {
    return (int)((float) rand() / RAND_MAX * (max - min + 1) + min);
}
