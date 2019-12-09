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
    int port = atoi(argv[2]); //TODO: *breaths in* SIZE CHECK. 
    
    if (port <= 4096) {
        printf("Error: The port should be above 4096.\n");
        return -1;
    }

    //Initialize socket structs 
    struct sockaddr_in serv_addr = {  
        .sin_family = AF_INET,     
        .sin_addr.s_addr = inet_addr("127.0.0.1"), // 127.0.0.1 binds to the local address
        .sin_port = htons(port)
    };

    struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM
    };
    
    //Resolve the hostname
    struct addrinfo* info; 
    int addr_error;
    if ((addr_error = getaddrinfo(host, NULL, &hints, &info)) != 0) {
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
        printf("Socket could not be created.\n");
        return -1;
    }  
   
    //Try to connect to the server. Try again if it fails. 
    while(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("Error: Connection failed.\n");
		sleep(3);
	}

    int err, i;
    for (i = 0; i < 3; i++) {
        printf("Connecting...\n");
        err = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        if (connect < 0) {
            printf("Failed. Attempting again...\n");
        } else {
            break;
        }
    }
    freeaddrinfo(info);

    //We failed to connect on the last try. Abandon ship.
    if (err < 0) {
        printf("The server failed to connect. Aborting...\n");
        return -1;
    } else {
        printf("Successfully connected to the server!\n\n");
    }
    
    //Now that we're connected to the server, we can send messages.
    printf("Input messages to send to the server.\n");
    while (1) {
        printf("-> ");

        char* userInput;
        scanf("%m[^\n]", userInput); //Read up to the newline character with buffer allocation done for us.
        //Check if the user send nothing.
        if (userInput == NULL) {
            userInput = "\0";
        } 
        
        //Send the message to the server.
        //We can do some checks for if the user send the correct tags later.
        //For now just send the message over.
        send(sock, userInput, strlen(userInput) + 1, 0);

        //flush the input buffer
        fflush(stdin);
    }
}
