#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "DUMB.h" 

/* This method receives a socket file descriptor and an input string to modify
 * Read the socket's sent message and store it in msg
 * Return -1 for failure, 0 for no message, 1 for a successful read.
 */
int receiveMessage(int sock, char** msg) { 
   /* We dont know how long the message is going to be. 
    * So we need a dynamically growing buffer to make sure we read all of
    * the message we receive from the other end
    * Start with an arbitrary number of bytes
    * and grow it until we get all of the bytes we need.
    */
    const int START_LEN = 8;

    char* buffer = malloc(sizeof(char) * START_LEN);
    int length = START_LEN;

    char* nullTerminator;
    int bufferOffset = 0, bytesReceived = 0;

    do {
        //Recieve a number of bytes from the socket.
        bytesReceived = recv(sock, buffer + bufferOffset, length - bufferOffset, 0);
        if (bytesReceived < 0) {
            printf("Error: The message from socket %d could not be read.\n", sock);
            *msg = "";
            return -1;
        } else if (bytesReceived == 0) {
            *msg = "";
            return 0;
        }
        
        //Look for the nll terminator in the buffer.
        //We can't use strchr to search for it because strchr expects a String, which is defined by having the null terminator.
        //When we find the null terminator. We've recieved the end of the message.
        if ((nullTerminator = memchr(buffer + bufferOffset, '\0', length - bufferOffset)) == NULL) {
            //The null terminator isnt in the buffer. 
            //We need to recieve more bytes and for that we'll need more buffer space.
            length *= 2;
            char* tempBuffer = realloc(buffer, length);
            if (tempBuffer == NULL) {
                printf("Error: The message from socket %d could not be read.\n", sock);
                return -1;
            } else {
                buffer = tempBuffer;
            }
        }
        
        //Increment the offset for reading from the buffer so we don't overwrite what we've already read. 
        bufferOffset += bytesReceived;
    } while (nullTerminator == NULL); //Keep expanding the buffer and reading until we've gotten a null terminator to signify the end of the message.

    //We've reached the end of the server's message.
    //The buffer is gonna be a bit bigger than the message we received
    //So lets cut off the end of the buffer and copy the memory over to msg.
    *msg = calloc(strlen(buffer) + 1, sizeof(char));
    memcpy(*msg, buffer, strlen(buffer) + 1); 

    return 1; 
}
