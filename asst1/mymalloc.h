#ifndef _MYMALLOC_H
#define _MYMALLOC_H

#define malloc(x) mymalloc(x, __FILE__, __LINE__)
#define free(x) myfree(x, __FILE__, __LINE__)

#define META_ID 6
#define BLOCK_SIZE 4096

static char myblock[4096];

void *mymalloc(int, char *, int);
int myfree(void *, char *, int);

#endif
