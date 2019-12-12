#ifndef _DUMB_H
#define _DUMB_H
#include <pthread.h>

#define SUCCESS "OK!"
#define ERR_EXISTENCE "ER:EXIST"
#define ERR_MALFORMED "ER:WHAT?"
#define ERR_NO_OPEN "ER:NOOPN"
#define ERR_NOEXIST "ER:NEXST"
#define ERR_NOTEMPTY "ER:NOTMT"
#define ERR_OPENED "ER:OPEND"
#define ERR_EMPTY "ER:EMPTY"

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
