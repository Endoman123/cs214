#ifndef _MYMALLOC_H
#define _MYMALLOC_H

#define malloc(x) mymalloc(x, __FILE__, __LINE__)
#define free(x) myfree(x, __FILE__, __LINE__)

static char myblock[4096];

void *mymalloc(unsigned short, char *, int);
int myfree(void *, char *, int);

#endif
