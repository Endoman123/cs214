#include <stdio.h>
#include <stdlib.h>

#include "mymalloc.h"

#define META_ID 6
#define BLOCK_SIZE 4096

/**
 * Metadata struct using bit fields.
 * This takes up exactly 2 bytes or 16 bits of space. 
 */
typedef struct _metadata {
    unsigned short s_userdata : 12;
    unsigned short inUse : 1;
    unsigned short identifier : 3;
} Metadata;

Metadata *getMetadata(unsigned short);
int setMetadata(unsigned short, unsigned short);
int printError(char *, char *, char *, int);

/**
 * Creates an allocation in the static block.
 * Returns a pointer if the allocation was successful,
 * NULL otherwise
 */
void *mymalloc(unsigned short size, char *file, int nLine) {
    void *ret = NULL;
     
    // Make sure the user is not trying to do anything stupid
    // I.E.: Don't let them allocate > 1 byte.
    if (size > 0) {    
        int i = 0;
        Metadata *curMD = getMetadata(i);
        
        // Initialize myblock if it hasn't been done already
        if (curMD->identifier != META_ID) {
            curMD->inUse = 0;
            curMD->identifier = META_ID;
            curMD->s_userdata = BLOCK_SIZE - sizeof(Metadata);
        }
        
        // First-fit the allocation;
        // that is, we need to find the first qualified empty block in our
        // memory block
        do {
            curMD = getMetadata(i);
            
            if (!curMD->inUse) {
                ret = &myblock[i + sizeof(Metadata)];

                curMD->inUse = 1;
                curMD->identifier = META_ID;

                // Consider breaking the contiguous block if possible
                // The new block size will be the difference between the current block's
                // userdata size, the needed allocation size, and the size of metadata.
                if (curMD->s_userdata - size > sizeof(Metadata)) {
                    unsigned short newUserSize = curMD->s_userdata - size - sizeof(Metadata);
                   
                    curMD->s_userdata = size;
                    
                    Metadata *nextMD = getMetadata(i + sizeof(Metadata) + size);
                    nextMD->inUse = 0;
                    nextMD->identifier = META_ID;
                    nextMD->s_userdata=newUserSize; 
                }
            } else
                i += sizeof(Metadata) + curMD->s_userdata;
        } while (ret == NULL && i < BLOCK_SIZE - sizeof(Metadata)); 
    }

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
Metadata *getMetadata(unsigned short offset) {
    if (offset >= BLOCK_SIZE)
        return NULL;
    
    return (Metadata *)(myblock + offset); 
}

/**
 * Sets the block of metadata in the char block
 */
int setMetadata(unsigned short offset, unsigned short newBlocksize) {
    Metadata md = {newBlocksize, 1, META_ID};

    *getMetadata(offset) = md;

    return 0; 
}

int printError(char* error, char* description, char* file, int nLine) {
    fprintf(stderr, "%s:%n: %s: %d\n", file, nLine, error, description);

    return 0;
}
