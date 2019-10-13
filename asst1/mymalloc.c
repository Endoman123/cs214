#include <stdlib.h>
#include <math.h>

#include "mymalloc.h"

/**
 * Creates an allocation in the static block.
 * Returns a pointer if the allocation was successful,
 * NULL otherwise
 */
void *mymalloc(int size, char *file, int nLine) {
    void *ret = NULL;
    
    // Check remaining data
    
    return ret; 
}

/**
 * Frees the memory location from the block.
 * Returns 1 if successful, 0 otherwise.
 */
int myfree(void *pointer, char *file, int nLine) {
    *(unsigned long *)pointer = setBit(*(unsigned long *)pointer, 0, 0);

    return 0;
}

/**
 * Metadata struct using bit fields.
 * This takes up exactly 2 bytes or 16 bits of space. 
 */ 
typedef struct _metadata_ {
    unsigned short blocksize : 12;
    unsigned short in_use : 1;
    unsigned short identifier : 3;
} Metadata;
