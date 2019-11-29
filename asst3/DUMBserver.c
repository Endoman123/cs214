#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

const char* IP_ADDR = "127.0.0.1";

int main(int argc, char **argv) {
    //User input for the server should be a port number.
    if (argc != 2) {
        printf("Error: Incorrect number of arguments.\n");
    }
    
    int port = atoi(argv[1]); //Parse the port from the user input
     
    //Create server socket
    int serv_sock = socket(AF_INET, SOCK_STREAM, 0); 

    //Bind socket
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET; //Use IPv4
    serv_addr.sin_addr.s_addr = inet_addr(IP_ADDR); //Set IP address
    serv_addr.sin_port = htons(port); //Set port
    
    //Listen for client connection
    listen(serv_sock, 20);

    //Receive connection
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_size);

    // Test message
    char str[] = "Hello World!";
    write(clnt_sock, str, sizeof(str));

    // Close
    close(clnt_sock);
    close(serv_sock);

    return 0;    
}
