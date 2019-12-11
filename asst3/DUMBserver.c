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
#include "DUMB.h"

#define IP_ADDR "127.0.0.1"

#define MAX_QUEUE 20

#define MALFORMED_ERROR "ER:WHAT?"

__thread messageBox openBox; // Thread local variable for which box is open.

void* handleClient(void*);

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

    struct addrinfo* info;
    int addr_error;
    if ((addr_error = getaddrinfo(NULL, port, &hints, &info)) != 0) {
        printf("Error: Could not obtain address information.\n");
        return -1;
    }

    int error;
    if ((error = bind(sock, info -> ai_addr, info -> ai_addrlen)) < 0) {
        printf("Error: Could not bind socket to port.\n");
        return -1;
    }
      
    freeaddrinfo(info);
    
    //Set the socket to listen for connections.
    //We also have to specify the maximum number of connections that can be queued for listening
    listen(sock, MAX_QUEUE);

    //All of our interactions with the client have to be concurrent with other clients
    //For this, lets have threads do the concurrency 
    
    struct sockaddr_storage conn_addr;
    int addressSize = sizeof(conn_addr);
    int conn;
    pthread_t thread;
    printf("Looking for connections from clients...\n");
    while ((conn = accept(sock, (struct sockaddr*)&conn_addr, &addressSize))) {
        int* client = malloc(sizeof(int));
        *client = conn;
        
        pthread_create(&thread, NULL, handleClient, (void*)client);
        pthread_detach(thread); 
    }
}

void* handleClient(void* args) {
    //Get the socket from the args
    int sock = *((int*) args);

    while (1) {
        char* clientMessage;
        char* serverResponse;
        int error = receiveMessage(sock, &clientMessage);

        if (error > 0 && clientMessage != NULL && clientMessage != "") {
            //Parse the information given to us by the client. 
            char* cmd = strtok(clientMessage, " !");
        
            //Every message is delimited by spaces except for put which for some reason delimits by !s.
            if (strcmp(cmd, "PUTMG") == 0) {
                int strLen = atoi(strtok(NULL, "!"));   
                char* args = strtok(NULL, "!");
                serverResponse = "TODO";
            } else {
                char* args = strtok(NULL, " ");

                //Check for the command
                if (strcmp(cmd, "HELLO") == 0) {
                    serverResponse = "Hello DUMBv0 ready!";
                    printf("Socket %d has connected.\n", sock);
                }
                else if (strcmp(cmd, "GDBYE") == 0) {
                    printf("Socket %d has disconnected.\n", sock);
                    return;
                }
                else if (strcmp(cmd, "CREAT") == 0) { 
                    if (args != NULL && strcmp(args, "") != 0) {
                        //Create the mailbox with the name in args
                        //Check for existence error.
                        serverResponse = "TODO";
                    } else {
                        serverResponse = MALFORMED_ERROR;
                    }
                }
                else if (strcmp(cmd, "OPNBX") == 0) { 
                    if (args != NULL && strcmp(args, "") != 0) {
                        serverResponse = "TODO";
                    } else {
                        serverResponse = MALFORMED_ERROR;
                    }
                }   
                else if (strcmp(cmd, "NXTMG") == 0) {
                    if (args == NULL || strcmp(args, "") == 0) {
                        serverResponse = "TODO";
                    } else {
                        serverResponse = MALFORMED_ERROR;
                    }
                }       
                else if (strcmp(cmd, "DELBX") == 0) {
                    if (args != NULL && strcmp(args, "") != 0) {
                        serverResponse = "TODO";
                    } else {
                        serverResponse = MALFORMED_ERROR;
                    }
                }       
                else if (strcmp(cmd, "CLSBX") == 0) {
                    if (args != NULL && strcmp(args, "") != 0) {
                        serverResponse = "TODO";
                    } else {
                        serverResponse = MALFORMED_ERROR;
                    }    
                } else {
                    serverResponse = MALFORMED_ERROR;
                }
            } 
        } else if (error < 0) return;

        send(sock, serverResponse, strlen(serverResponse) + 1, 0);
        free(clientMessage);
    }
}
