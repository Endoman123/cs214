#ifndef _MYMALLOC_H
#define _MYMALLOC_H

#define malloc(x) mymalloc(x, __FILE__, __LINE__)
#define free(x) myfree(x, __FILE__, __LINE__)

#define META_ID 6
#define BLOCK_SIZE 4096

static char myblock[4096];

/**
 * Metadata struct using bit fields.
 * This takes up exactly 2 bytes or 16 bits of space. 
 */
typedef struct _metadata {
    unsigned short blocksize : 12;
    unsigned short in_use : 1;
    unsigned short identifier : 3;
} Metadata;

void *mymalloc(int, char *, int);
int myfree(void *, char *, int);
Metadata getMetadata(int);

#endif
