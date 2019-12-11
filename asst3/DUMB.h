#ifndef _DUMB_H
#define _DUMB_H
#include <pthread.h>

typedef struct _messageBox {
    char *name;
    struct _message* msg;
    struct _messageBox* next;
    pthread_mutex_t mutex_lock;
} messageBox;

typedef struct _message {
    char* msg;
    struct _message* next; 
} message;

typedef struct _threadArgs {
    int sock;
    char* ip;
} threadArgs;

int receiveMessage(int, char**);

#endif
