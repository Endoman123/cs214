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

int main(int argc, char* argv[]) {
    //The user should input an ip address and a port.
    if (argc != 3) {
        printf("Error: Incorrect number of arguments for the client.\n");
        return -1;
    }

    char* host = argv[1];
    char* port = argv[2]; 
    
    if (atoi(port) <= 4096) {
        printf("Error: The port should be above 4096.\n");
        return -1;
    }

    struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM
    };
    
    //Resolve the hostname
    struct addrinfo* info; 
    int addr_error;
    if ((addr_error = getaddrinfo(host, port, &hints, &info)) != 0) {
        printf("Error: Hostname could not be resolved.\n");
        return -1;
    }

    /*for (struct addrinfo* iter = info; iter != NULL; iter = iter -> ai_next) {
        getnameinfo(iter -> ai_addr, iter -> ai_addrlen, host, strlen(host) * sizeof(char), NULL, 0, NI_NUMERICHOST);
		inet_aton(host, &serv_addr.sin_addr);
    }*/

     //Create the client socket
    int sock = 0;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error: Socket could not be created.\n");
        return -1;
    }  
   
    int error, i;
    for (i = 0; i < 3; i++) {
        printf("Connecting...\n");
        error = connect(sock, info -> ai_addr, info -> ai_addrlen);
        if (error < 0) {
            printf("Failed. Attempting again...\n");
        } else {
            break;
        }
    }
    freeaddrinfo(info);

    //We failed to connect on the last try. Abandon ship.
    if (error < 0) {
        printf("The server failed to connect. Aborting...\n");
        return -1;
    } else {
        printf("Successfully connected to the server!\n\n");
    }
    
    //Now that we're connected to the server, we can send messages.
    while (1) {
        printf("Send a message: ");

        char* userInput;
        scanf("%m[^\n]", &userInput); //Read up to the newline character with buffer allocation done for us.

        //Check if the user sent nothing.   
        if (userInput == NULL) userInput = "\0"; 

        //Send the message to the server.
        send(sock, userInput, strlen(userInput) + 1, 0);   
        
        char* servResponse;
        receiveServerMsg(sock, &servResponse);

        if (servResponse != NULL && servResponse != "") {
            printf("Server: %s\n", servResponse);
        } else {
            printf("Disconnecting from the server...\n");
            break;
        }
        //flush the input buffer.  
        char ch; while ((ch = getchar()) != '\n');

        //free(userInput);
        //free(servResponse);
    }
}

//Input the string to receive the server message
//Return the length of that string.
int receiveServerMsg(int servSocket, char** msg) {
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
        //printf("Recv() with arguments %d, %d, %d, %d\n", servSocket, buffer + bufferOffset, length - bufferOffset, 0);
        bytesReceived = recv(servSocket, buffer + bufferOffset, length - bufferOffset, 0);
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
}
