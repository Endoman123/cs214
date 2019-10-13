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
    
    // First-fit the allocation;
    // that is, we need to find the first qualified empty block in our
    // memory block
    Metadata *curMD = NULL;
    int i = 0;
    do {
        curMD = getMetadata(i);
        
        if (!curMD->inUse) {
            ret = &myblock[i + sizeof(Metadata)];

            curMD->inUse = 1;
            curMD->blocksize = size;
            curMD->identifier = META_ID; 
        } else
            i += curMD->blocksize + sizeof(Metadata);
    } while (ret == NULL);

    return ret; 
}

/**
 * Frees the memory location from the block.
 * Returns 1 if successful, 0 otherwise.
 */
int myfree(void *pointer, char *file, int nLine) {
    // *(unsigned long *)pointer = setBit(*(unsigned long *)pointer, 0, 0);

    return 0;
}

/**
 * Gets the block of metadata from the char block
 */
Metadata *getMetadata(int offset) {
    return (Metadata *)(myblock + offset); 
}

/**
 * Sets the block of metadata in the char block
 */
int setMetadata(int offset, int newBlocksize) {
    Metadata md = {newBlocksize, 1, META_ID};

    *getMetadata(offset) = md;

    return 0; 
}
