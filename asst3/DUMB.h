#ifndef _DUMB_H
#define _DUMB_H
#include <pthread.h>

typedef struct _messageBox {
    struct _message* msg;
    struct _messageBox* next;
    pthread_mutex_t mutex_lock;
    char *name;
} messageBox;

typedef struct _message {
    struct _message* next; 
    char* msg;
} message;

typedef struct _threadArgs {
    int sock;
    char* ip;
} threadArgs;

int receiveMessage(int, char**);

#endif
