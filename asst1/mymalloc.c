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
    *(unsigned long *)pointer = setBit(*(unsigned long *)pointer, 0, 0); 
    
    return 0;
}

/**
 * Given the pointer and offset, 
 * sets the bit to the specified value.
 */
unsigned long setBit(unsigned long bytes, int value, int offset) {
    return (unsigned long)((bytes & ~(1 << offset)) | (value << offset));
}
