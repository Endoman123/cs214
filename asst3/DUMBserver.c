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

const char* IP_ADDR = "127.0.0.1";

const int MAX_QUEUE = 20;

void* handleClient(void*);
int receiveClientMessage(int, char**);

int main(int argc, char **argv) {
    //User input for the server should be a port number.
    if (argc != 2) {
        printf("Error: Incorrect number of arguments.\n");
    }
    
    char* port = argv[1]; //Parse the port from the user input
    
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
        int strLen = receiveClientMessage(sock, &clientMessage);
  
        if (clientMessage != NULL && clientMessage != "") {
            printf("The client has sent a message!\n");
            printf("Client %d sent the message \"%s\"\n", sock, clientMessage);
        }

        char* serverResponse;
        serverResponse = "Message received";
        send(sock, serverResponse, strlen(serverResponse) + 1, 0);
    }
}

int receiveClientMessage(int clientSocket, char** msg) {
    //We dont know how long the message is going to be. 
    //So we need a dynamically growing buffer to make sure we read all of
    //the message we receive from the other end
    //Start with an arbitrary number of bytes
    //and grow it until we get all of the bytes we need.
    
    const int START_LEN = 8;
    const int MAX_BUFFER_SIZE = 2048;

    char* buffer = malloc(sizeof(char) * START_LEN);
    int length = START_LEN;

    char* nullTerminator;
    int bufferOffset = 0, bytesReceived = 0;

    do {
        //printf("Recv() with arguments %d, %d, %d, %d\n", clientSocket, buffer + bufferOffset, length - bufferOffset, 0);
        bytesReceived = recv(clientSocket, buffer + bufferOffset, length - bufferOffset, 0);
        if (bytesReceived < 0) {
            printf("Error: The message from the server could not be read\n");
            *msg = "";
            return;
        } else if (bytesReceived == 0) {
            *msg = "";
            return;
        }


        if ((nullTerminator = memchr(buffer + bufferOffset, '\0', length - bufferOffset)) == NULL && length < MAX_BUFFER_SIZE) {
            length *= 2;
            char* tempBuffer = realloc(buffer, length);
            if (tempBuffer == NULL) {
                printf("Error: The message from the server could not be read.\n");
            } else {
                buffer = tempBuffer;
            }
        } else if (length < MAX_BUFFER_SIZE) break; 
        bufferOffset += bytesReceived;
    } while (nullTerminator == NULL);

    //We've reached the end of the server's message.
    //The buffer is gonna be a bit bigger than the message we received
    //So lets cut off the end of the buffer and copy the memory over to msg.
    *msg = calloc(strlen(buffer) + 1, sizeof(char));
    memcpy(*msg, buffer, strlen(buffer) + 1); 

    return strlen(*msg);
}
