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
#include "DUMB.h"

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
    
    //Start the connection with the server by sending a HELLO.
    send(sock, "HELLO", 6, 0);   
    char* hello;
    receiveMessage(sock, &hello);
    printf("%s\n", hello);
    
    //Now that we're connected to the server, we can send messages.
    while (1) {
        printf("[%s] dumb > ", host);

        char* userInput;
        scanf("%m[^\n]", &userInput); //Read up to the newline character with buffer allocation done for us.

        //Check if the user sent nothing.   
        if (userInput == NULL) userInput = "\0"; 
 
        //Compare the user input to the commands available.       
        char* command;
        char* arg;
        if (strcmp(userInput, "quit") == 0) {
            command = "GDBYE";    
        } 
        else if (strcmp(userInput, "create") == 0) {
            printf("Enter the name of the new message box: ");
            scanf(" %m[^\n]", &arg);
            asprintf(&command, "CREAT %s", arg); 
        }
        else if (strcmp(userInput, "open") == 0) {
            printf("Enter the message box to open: "); 
            scanf(" %m[^\n]", &arg);
            printf("%s\n", arg);
            asprintf(&command, "OPNBX %s", arg);
            printf("%s\n", command);
        } 
        else if (strcmp(userInput, "next") == 0) {
            command = "NXTMG";
        }
        else if (strcmp(userInput, "put") == 0) {
            printf("Enter your message: "); 
            scanf(" %m[^\n]", &arg);
            asprintf(&command, "PUTMG %s", arg);
        }
        else if (strcmp(userInput, "delete") == 0) {
            printf("Enter the name of the box to delete: "); 
            scanf(" %m[^\n]", &arg);
            asprintf(&command, "DELBX %s", arg);
        }
        else if (strcmp(userInput, "close") == 0) {
            printf("Enter the name of the box to close: "); 
            scanf(" %m[^\n]", &arg);
            asprintf(&command, "CLSBX %s", arg);
        }        
        else {
            command = "";
            printf("Invalid command.\n");
        }

        if (command != NULL && command != "") {
            //Send the message to the server.
            send(sock, command, strlen(command) + 1, 0);   
            
            if (strcmp(command, "GDBYE") == 0) break; 
            char* servResponse;
            receiveMessage(sock, &servResponse);

            if (servResponse != NULL && servResponse != "") {
                printf("Server: %s\n", servResponse);
            } else {
                printf("Disconnecting from the server...\n");
                break;
            }
        }

        //flush the input buffer.  
        char ch; while ((ch = getchar()) != '\n' && ch != EOF);

        //free(userInput);
        //free(servResponse);
    }
}

