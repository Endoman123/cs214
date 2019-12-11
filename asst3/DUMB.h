#ifndef _DUMB_H
#define _DUMB_H

typedef struct _messageBox {
    struct _message* msg;
    struct _messageBox* next;
    pthread_mutex_t mutex_lock;
} messageBox;

typedef struct _message {
    char* msg;
    struct _message* next; 
} message;

int receiveMessage(int, char**);

#endif
