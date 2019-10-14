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
int coaleseFreeBlocks();
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
    unsigned short index = (unsigned short)((unsigned long)pointer - (unsigned long)myblock) - 2; 

    // Make sure that the pointer is within the range of the block.
    // If it is not, it is safe to assume that it was not allocated by mymalloc().
    if (index >= 0 && index < BLOCK_SIZE) {
        Metadata *curMD = getMetadata(index);

        // Make sure that the block has metadata linked to it
        // and that the metadata is still marked "in use" 
        if (curMD->identifier != META_ID) {
            printError("Pointer error", "Pointer was not allocated by mymalloc()", file, nLine);
            return 0;
        } else if (curMD->inUse == 0) {
            printError("Redundant free error", "Block was already declared not in use", file, nLine);
            return 0;
        }

        // Mark this block as not in use
        curMD->inUse = 0;

        // Coalese all the unused blocks
        coaleseFreeBlocks();

        return 1; 
    }

    printError("Pointer error", "Pointer was not allocated by mymalloc()", file, nLine);
    return 0;
}

/**
 * Merges together contiguous free blocks
 */
int coaleseFreeBlocks() {
    int i = 0;
    Metadata *curMD = NULL;

    do {
        curMD = getMetadata(i);

        if (curMD->inUse == 0) {
            int j = i + sizeof(Metadata) + curMD->s_userdata;
            Metadata *tempMD = NULL;

            while (j < BLOCK_SIZE) {
                tempMD = getMetadata(j);
                
                if (tempMD->inUse)
                    break;
                
                curMD->s_userdata += sizeof(Metadata) + tempMD->s_userdata;
                j += tempMD->s_userdata + sizeof(Metadata);
            }
        }

        i += sizeof(Metadata) + curMD->s_userdata;
    } while (i < BLOCK_SIZE);

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
