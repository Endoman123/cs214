#include <stdlib.h>

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
    return 0;
}
