#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>

#include "DUMB.h"

#define IP_ADDR "127.0.0.1"

#define MAX_QUEUE 20

#define SUCCESS "OK!"
#define ERR_EXISTENCE "ER:EXIST"
#define ERR_MALFORMED "ER:WHAT?"
#define ERR_NO_OPEN "ER:NOOPN"
#define ERR_NOEXIST "ER:NEXST"
#define ERR_NOTEMPTY "ER:NOTMT"
#define ERR_OPENED "ER:OPEND"
#define ERR_EMPTY "ER:EMPTY"

messageBox* mailbox;

__thread messageBox* openBox; // Thread local variable for which box is open.

void* handleClient(void*);
int createMailbox(char *);
messageBox* getMailBox(char*);
char* getTime();

int main(int argc, char **argv) {
    //User input for the server should be a port number.
    if (argc != 2) {
        printf("Error: Incorrect number of arguments.\n");
    }
    
    char* port = argv[1]; //Parse the port from the user input
    
    //Stop SIGPIPES from stopping the server 
    //SIGPIPE occurs when a program tries to send or recv to or from a socket that is no longer available/connected
    //The default action for a SIGPIPE is to kill the program
    //We have to override this behavior so a SIGPIPE does nothing whatsoever.
    sigaction(SIGPIPE, &(struct sigaction){SIG_IGN}, NULL);

    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error: Socket could not be created.\n");
    }
    
    struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM 
    };
        
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(atoi(port))
    };

    int enable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        printf("Error: Could not set socket options.\n");
        return -1;
    }
    
    int error;
    if ((error = bind(sock, (struct sockaddr*) &addr, sizeof(addr))) < 0) {
        printf("Error: Could not bind socket to port.\n");
        return -1;
    }
    
    //Set the socket to listen for connections.
    //We also have to specify the maximum number of connections that can be queued for listening
    listen(sock, MAX_QUEUE);

    //All of our interactions with the client have to be concurrent with other clients
    //For this, lets have threads do the concurrency 
    
    struct sockaddr_in conn_addr;
    int addressSize = sizeof(conn_addr);
    int conn;
    pthread_t thread;
    printf("Looking for connections from clients...\n");
    while ((conn = accept(sock, (struct sockaddr*)&conn_addr, &addressSize))) {
        char* ip = inet_ntoa(conn_addr.sin_addr);

        threadArgs args = {
            .sock = conn,
            .ip = ip
        };

        pthread_create(&thread, NULL, handleClient, (void*)&args);
        pthread_detach(thread); 
    }
}

void* handleClient(void* args) {
    //Get the socket from the args
    threadArgs* tArgs = (threadArgs*) args;
    int sock = tArgs -> sock;   
    char* ip = tArgs -> ip;

    while (1) {
        char* clientMessage;
        char* serverResponse;
        char* cmdError = NULL;
        int error = receiveMessage(sock, &clientMessage);

        if (error > 0 && clientMessage != NULL && clientMessage != "") {
            //Parse the information given to us by the client. 
            char* cmd = strtok(clientMessage, " !");

            char* time = getTime();
            char* stamp;
            asprintf(&stamp, "%s %s", time, ip);

            //Every message is delimited by spaces except for put which for some reason delimits by !s.

            //Check for the command
            if (strcmp(cmd, "HELLO") == 0) {
                char* args = strtok(NULL, " ");
                serverResponse = "HELLO DUMBv0 ready!";
                printf("%s connected\n", stamp);
            }
            else if (strcmp(cmd, "GDBYE") == 0) {
                char* args = strtok(NULL, " ");
                printf("%s disconnected\n", stamp);

                //Close before shutdown
                if (openBox != NULL) {
                    pthread_mutex_unlock(&(openBox -> mutex_lock));    
                }

                shutdown(sock, 2); //Shut down all sends and receives.
                return;
            }
            else if (strcmp(cmd, "CREAT") == 0) { 
                char* args = strtok(NULL, " ");
                if (args != NULL && strcmp(args, "") != 0) {
                    //serverResponse = createMailbox(args) ? SUCCESS : ERR_EXISTENCE;
                    if (createMailbox(args)) {
                        serverResponse = SUCCESS;
                    } else {
                        cmdError = ERR_EXISTENCE;
                    }
                } else {
                    cmdError = ERR_MALFORMED;
                }
            }
            else if (strcmp(cmd, "OPNBX") == 0) { 
                char* args = strtok(NULL, " ");
                if (args != NULL && strcmp(args, "") != 0) {
                    messageBox* opn = getMailBox(args);
                    
                    if (opn != NULL) {
                        //Check if the box is opened by another thread with mutexing.
                        if (pthread_mutex_trylock(&(opn -> mutex_lock)) == 0) {
                            //The mutex is open and we've locked it.
                            openBox = opn;
                        } else {
                            cmdError = ERR_OPENED;
                        }
                    } else {
                        cmdError = ERR_NOEXIST; 
                    }
                } else {
                    cmdError = ERR_MALFORMED;
                }
            }   
            else if (strcmp(cmd, "NXTMG") == 0) {
                char* args = strtok(NULL, " ");
                if (args == NULL || strcmp(args, "") == 0) {
                    if (openBox == NULL) {
                        cmdError = ERR_NO_OPEN;
                    } else {
                        //Get the next message.
                        if (openBox -> msg == NULL) {
                            cmdError = ERR_EMPTY; 
                        } else {
                            char* ret = openBox -> msg -> msg;
                            asprintf(&serverResponse, "OK!%d!%s", strlen(ret), ret);
                            openBox -> msg = openBox -> msg -> next;
                        }
                    }   
                } else {
                    cmdError = ERR_MALFORMED;
                }
            }       
            else if (strcmp(cmd, "PUTMG") == 0) {
                int strLen = atoi(strtok(NULL, "!"));   
                char* args = strtok(NULL, "!");
            
                if (openBox == NULL) {
                    cmdError = ERR_NO_OPEN;
                } else {
                    //Check for a crappy strLen
                    if (strLen != strlen(args)) {
                        cmdError = ERR_MALFORMED;
                    } else {
                        //Check if the head is initialized
                        if (openBox -> msg == NULL) {
                            openBox -> msg = malloc(sizeof(message));
                            asprintf(&(openBox -> msg -> msg), "%s", args);

                            openBox -> msg -> next = NULL;
                        } else {
                            //Iterate until the end.
                            message *iter, *tail;
                            for (iter = openBox -> msg; iter != NULL; iter = iter -> next) {
                                tail = iter;
                            }

                            iter = malloc(sizeof(message));
                            asprintf(&(iter -> msg), "%s", args);
                            iter -> next = NULL;

                            tail -> next = iter;
                        }       
                        asprintf(&serverResponse, "OK!%d", strLen);
                    }
                }
            }
            else if (strcmp(cmd, "DELBX") == 0) {
                char* args = strtok(NULL, " ");
                if (args != NULL && strcmp(args, "") != 0) {
                    int delerr = deleteMailBox(args);

                    if (delerr == 0) {
                        serverResponse = SUCCESS;
                    } else if (delerr == -1) {
                        cmdError = "ER:NEXTST"; 
                    } else if (delerr = -2) {
                        cmdError = ERR_OPENED;
                    } else if (delerr = -3) {
                        cmdError = ERR_NOTEMPTY; 
                    }
                }
            }       
            else if (strcmp(cmd, "CLSBX") == 0) {
                char* args = strtok(NULL, " ");
                if (args != NULL && strcmp(args, "") != 0) {
                    if (openBox != NULL && strcmp(openBox -> name, args)) {
                        pthread_mutex_unlock(&(openBox -> mutex_lock));
                        openBox = NULL;

                        serverResponse = SUCCESS;
                    } else {
                        cmdError = ERR_NO_OPEN;
                    }
                } else {
                    cmdError = ERR_MALFORMED;
                }    
            } else {
                cmdError = ERR_MALFORMED;
            } 

            printf("%s %s\n", stamp, cmd);

            if (cmdError != NULL) {
                printf("%s %s\n", stamp, cmdError);
            }   
             
            free(time);
            free(stamp);
            free(clientMessage);
        } else if (error < 0) return;
        
        if (cmdError != NULL) {
            send(sock, cmdError, strlen(cmdError) + 1, 0);  
        } else {
            send(sock, serverResponse, strlen(serverResponse) + 1, 0);
        }
    }
}   
/**
 * Creates a mailbox for a connected client
 */
int createMailbox(char *name) {
    if (mailbox == NULL) {
        mailbox = malloc(sizeof(messageBox));
        mailbox -> name = malloc(strlen(name) + 1);
        asprintf(&(mailbox -> name), "%s", name);
        mailbox -> msg = NULL; 
        mailbox -> next = NULL;
        pthread_mutex_init(&(mailbox -> mutex_lock), NULL); 
        return 1;
    } else {
        messageBox *ptr, *tail;
        for (ptr = mailbox; ptr != NULL; ptr = ptr -> next) {
            tail = ptr;

            if (strcmp(ptr -> name, name) == 0) {
                return 0;
            }
        }

        if (ptr == NULL) {
            ptr = malloc(sizeof(messageBox));
            ptr -> name = malloc(strlen(name) + 1);
            asprintf(&(ptr -> name), "%s", name);
            ptr -> msg = NULL;
            ptr -> next = NULL;
            pthread_mutex_init(&(ptr -> mutex_lock), NULL); 
            
            tail -> next = ptr;
            return 1;
        }
    }
}

messageBox* getMailBox(char* name) {
    messageBox* iter;
    for (iter = mailbox; iter != NULL; iter = iter -> next) {
        if (strcmp(iter -> name, name) == 0) return iter;
    }
    return NULL;
} 

int deleteMailBox(char* name) {
    messageBox* prev = NULL;
    messageBox* del = NULL;

    //Find the box to delete.
    //Could be the head or somewhere else.
    if (strcmp(mailbox -> name, name) == 0) {
        del = mailbox;
    } else {
        messageBox* iter;
        for (iter = mailbox; iter != NULL; iter = iter -> next) {
            if (iter -> next != NULL && strcmp(iter -> next -> name, name) == 0) {
                //Found the box to delete        
                prev = iter;
                del = iter -> next;
                break;
            }
        }   
    }
    
    if (del == NULL) return -1; //ER:NEXST

    if (pthread_mutex_trylock(&(mailbox -> mutex_lock)) == 0) {
        //Head is not locked;
        pthread_mutex_unlock(&(mailbox -> mutex_lock));

        //Check if there's a message.
        if (mailbox -> msg == NULL) {
            if (del = mailbox) {
                mailbox = mailbox -> next;
                free(mailbox);
            } else {
                prev -> next = del -> next;
                free(del);           
            }
            return 0;
        } else {
            //NOTMT
            return -3;
        }
    } else {
        //OPEND
        return -2;
    }
}

char* getTime() {
    time_t rawTime;
    struct tm* tmp;

    char* buffer = malloc(sizeof(char) * 50);

    time(&rawTime);

    tmp = localtime(&rawTime);

    strftime(buffer, 50, "%H%M %d %b", tmp);
    return buffer;
}


