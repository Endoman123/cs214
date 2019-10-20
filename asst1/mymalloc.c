#include <stdarg.h>
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
int printError(char *, int, char *, char *, ...);

/**
 * Creates an allocation in the static block.
 * Returns a pointer if the allocation was successful,
 * NULL otherwise
 */
void *mymalloc(unsigned short size, char *file, int nLine) {
    void *ret = NULL;
     
    // Make sure the user is not trying to do anything stupid
    // I.E.: Don't let them allocate > 1 byte.
    if (size > 0 && size <= BLOCK_SIZE - sizeof(Metadata)) {    
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
           
            // If the current block fits the allocation requirement,
            // put it in. 
            if (!curMD->inUse && curMD->s_userdata >= size) {
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
   
        if (ret == NULL) 
            printError(file, nLine, "Saturation of dynamic memory", "Not enough space to allocate %d bytes", size);
    } else
        printError(file, nLine, "Invalid argument", "Size is not within range of 0 < x < 4096");

    return ret; 
}

/**
 * Frees the memory location from the block.
 * Returns 1 if successful, 0 otherwise.
 */
int myfree(void *pointer, char *file, int nLine) {
    unsigned short index = (unsigned short)((unsigned long)pointer - (unsigned long)myblock) - 2; 

    // Make sure that the pointer not null and that it is within the range of the block.
    // If it is not, it is safe to assume that it was not allocated by mymalloc().
    if (pointer == NULL) {
        printError(file, nLine, "Pointer error", "Argument 0 is not a pointer");
    } if (index >= 0 && index < BLOCK_SIZE) {
        Metadata *curMD = getMetadata(index);

        // Make sure that the block has metadata linked to it
        // and that the metadata is still marked "in use" 
        if (curMD->identifier != META_ID) {
            printError(file, nLine, "Pointer error", "Pointer was not allocated by mymalloc()");
            return 0;
        } else if (curMD->inUse == 0) {
            printError(file, nLine, "Redundant free error", "Block was already declared not in use");
            return 0;
        }

        // Mark this block as not in use
        curMD->inUse = 0;

        // Coalese all the unused blocks
        coaleseFreeBlocks();

        return 1; 
    }

    printError(file, nLine, "Pointer error", "Pointer was not allocated by mymalloc()");
    return 0;
}

/**
 * Merges together contiguous free blocks
 */
int coaleseFreeBlocks() {
    int i = 0;
    Metadata *curMD = NULL;

    do {
        // Get the current metadata for the allocation
        curMD = getMetadata(i);

        // If it's not in use, 
        // we need to start checking ahead until we find a block in use
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

        // Increment by the blocksize (including meta)
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

/**
 * Prints an error to standard error output
 */
int printError(char* file, int nLine, char* error, char* descFormat, ...) { 
    char description[999];
    va_list argp;
    va_start(argp, descFormat);

    vsnprintf(description, 999 * sizeof(char), descFormat, argp);
    fprintf(stderr, "%s:%d: %s: %s\n", file, nLine, error, description);

    return 0;
}
