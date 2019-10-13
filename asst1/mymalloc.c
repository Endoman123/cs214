#include <stdlib.h>
#include <math.h>

#include "mymalloc.h"

/**
 * Metadata struct using bit fields.
 * This takes up exactly 2 bytes or 16 bits of space. 
 */
typedef struct _metadata {
    unsigned short blocksize : 12;
    unsigned short inUse : 1;
    unsigned short identifier : 3;
} Metadata;

Metadata *getMetadata(int);
int setMetadata(int, int);

/**
 * Creates an allocation in the static block.
 * Returns a pointer if the allocation was successful,
 * NULL otherwise
 */
void *mymalloc(int size, char *file, int nLine) {
    void *ret = NULL;
    
    // Make sure the user is not trying to do anything stupid
    // I.E.: Don't let them allocate > 1 byte,
    // don't let them allocate < 4096 bytes (including metadata)
    if (size < 1) {
    }
    
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
