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
        //Start the connection with the server by sending a HELLO.
        send(sock, "HELLO", 6, 0);   
        char* hello;
        receiveMessage(sock, &hello);
        printf("%s\n", hello);
        //Check if the server sent back the correct string.
        if (strcmp(hello, "HELLO DUMBv0 ready!") != 0) {
            printf("The server did not respond back correctly.\n"); 
            printf("Aborting connection...\n");
            return -1;
        }
    }
    
    //Now that we're connected to the server, we can send messages.
    while (1) {
        printf("[%s] dumb > ", host);

        char* userInput;
        char* servResponse;
        scanf("%m[^\n]", &userInput); //Read up to the newline character with buffer allocation done for us.

        //Check if the user sent nothing.   
        if (userInput == NULL) userInput = "\0"; 
 
        //Compare the user input to the commands available.       
        char* command;
        char* arg;
        if (strcmp(userInput, "quit") == 0) {
            command = "GDBYE";    
            send(sock, command, strlen(command) + 1, 0);   
            receiveMessage(sock, &servResponse);
            if (servResponse == NULL || strcmp(servResponse, "") == 0) {
                return;
            } else {
                printf("The server responded unexpectedly.\n");
                printf("Aborting quit...\n");
                printf("The server sent back \"%s\"", servResponse);
            }
        } 
        else if (strcmp(userInput, "create") == 0) {
            printf("Enter the name of the new message box: ");
            scanf(" %m[^\n]", &arg);
            asprintf(&command, "CREAT %s", arg); 
            send(sock, command, strlen(command) + 1, 0);   
            receiveMessage(sock, &servResponse);

            if (servResponse == NULL || strcmp(servResponse, "") == 0) {
                printf("No response from the server\n");
                return;
            } 
            else if (strcmp(servResponse, "ER:EXIST") == 0) {
                printf("The box already exists!\n");
            } 
            else if (strcmp(servResponse, "ER:WHAT?") == 0) {
                printf("Your message is in some way broken or malformed.\n");
            } 
            else if (strcmp(servResponse, "OK!") == 0) {
                printf("The box was created successfully!\n");
            }
            else {
                printf("Unknown error\n");
            }
        }
        else if (strcmp(userInput, "open") == 0) {
            printf("Enter the message box to open: "); 
            scanf(" %m[^\n]", &arg);
            asprintf(&command, "OPNBX %s", arg);
            send(sock, command, strlen(command) + 1, 0);   
            receiveMessage(sock, &servResponse);

            if (servResponse == NULL || strcmp(servResponse, "") == 0) {
                printf("No response from the server\n");
                return;
            } 
            else if (strcmp(servResponse, "ER:NEXST") == 0) {
                printf("The box does not exist!\n");
            } 
            else if (strcmp(servResponse, "ER:OPEND") == 0) {
                printf("The box is currently open!\n");
            } 
            else if (strcmp(servResponse, "ER:WHAT?") == 0) {
                printf("Your message is in some way broken or malformed.\n");
            } 
            else if (strcmp(servResponse, "OK!") == 0) {
                printf("The box was opened successfully!\n");
            }
            else {
                printf("Unknown error\n");
            }
        } 
        else if (strcmp(userInput, "next") == 0) {
            command = "NXTMG";
            send(sock, command, strlen(command) + 1, 0);   
            receiveMessage(sock, &servResponse);

            if (servResponse == NULL || strcmp(servResponse, "") == 0) {
                printf("No response from the server\n");
                return;
            } 
            else if (strcmp(servResponse, "ER:EMPTY") == 0) {
                printf("No messages left in the box!\n");
            } 
            else if (strcmp(servResponse, "ER:NOOPN") == 0) {
                printf("You currently don't have a box open\n");
            } 
            else if (strcmp(servResponse, "ER:WHAT?") == 0) {
                printf("Your message is in some way broken or malformed.\n");
            } 
            else {
                char* res = strtok(servResponse, "!");
                char* strLen = strtok(NULL, "!");
                char* msg = strtok(NULL, "!");

                if (res != NULL && strcmp(res, "OK") == 0 && msg != NULL && atoi(strLen) == strlen(msg)) {
                    printf("%s\n", msg);
                } else {
                    printf("Unknown error\n");
                }
            }
        }
        else if (strcmp(userInput, "put") == 0) {
            printf("Enter your message: "); 
            scanf(" %m[^\n]", &arg);
            int arglen = strlen(arg); 
            asprintf(&command, "PUTMG!%d!%s", arglen, arg);
            printf("Sending %s\n", command);
            send(sock, command, strlen(command) + 1, 0);   
            receiveMessage(sock, &servResponse);
 
            if (servResponse == NULL || strcmp(servResponse, "") == 0) {
                printf("No response from the server\n");
                return;
            } 
            else if (strcmp(servResponse, "ER:NOOPN") == 0) {
                printf("You currently don't have a box open\n");
            } 
            else if (strcmp(servResponse, "ER:WHAT?") == 0) {
                printf("Your message is in some way broken or malformed.\n");
            } else { 
               char* res = strtok(servResponse, "!");
               char* strLen = strtok(NULL, "!");

               if (res == NULL || strLen == NULL || (res != NULL && strcmp(res, "OK") != 0) || (strLen != NULL && atoi(strLen) != arglen)) {
                   printf("Unknown error\n");
               } 
               else {
                   printf("Message put successfully!\n");   
               }
            } 
        }
        else if (strcmp(userInput, "delete") == 0) {
            printf("Enter the name of the box to delete: "); 
            scanf(" %m[^\n]", &arg);
            asprintf(&command, "DELBX %s", arg);
            send(sock, command, strlen(command) + 1, 0);   
            receiveMessage(sock, &servResponse);
            
            if (servResponse == NULL || strcmp(servResponse, "") == 0) {
                printf("No response from the server\n");
                return;
            } 
            else if (strcmp(servResponse, "ER:NEXST") == 0) {
                printf("The box does not exist!\n");
            } 
            else if (strcmp(servResponse, "ER:OPEND") == 0) {
                printf("The box is currently open!\n");
            } 
            else if (strcmp(servResponse, "ER:WHAT?") == 0) {
                printf("Your message is in some way broken or malformed.\n");
            } 
            else if (strcmp(servResponse, "ER:NOTMT?") == 0) {
                printf("The box is not empty!\n");
            }
            else if (strcmp(servResponse, "OK!") == 0) {
                printf("The box was deleted successfully!\n"); 
            }
            else {
                printf("Unknown error\n");
            }
        }
        else if (strcmp(userInput, "close") == 0) {
            printf("Enter the name of the box to close: "); 
            scanf(" %m[^\n]", &arg);
            asprintf(&command, "CLSBX %s", arg);
            send(sock, command, strlen(command) + 1, 0);   
            receiveMessage(sock, &servResponse);
 
            if (servResponse == NULL || strcmp(servResponse, "") == 0) {
                printf("No response from the server\n");
                return;
            } 
            else if (strcmp(servResponse, "ER:NOOPN") == 0) {
                printf("You don't have that box open!\n");
            }
            else if (strcmp(servResponse, "ER:WHAT?") == 0) {
                printf("Your message is in some way broken or malformed.\n");
            } 
            else if (strcmp(servResponse, "OK!") == 0) {
                printf("The box has been closed!\n");
            }
            else {
                printf("Unknown error\n");
            }
        }        
        else {
            command = "";
            printf("Invalid command.\n");
        }

        //flush the input buffer.  
        char ch; while ((ch = getchar()) != '\n' && ch != EOF);

        //free(userInput);
        //free(servResponse);
    }
}

