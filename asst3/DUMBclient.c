#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    //The user should input an ip address and a port.
    if (argc != 3) {
        printf("Error: Incorrect number of arguments for the client.\n");
        return -1;
    }

    char* host = argv[1];
    int port = atoi(argv[2]); //TODO: *breaths in* SIZE CHECK. 
    
    if (port >= 4096) {
        printf("Error: The port should be above 4096.\n");
        return -1;
    }

    //Create the client socket
    int sock = 0;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket could not be created.\n");
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
    struct addrinfo* results; 
    int addr_error;
    if ((addr_error = getaddrinfo(host, NULL, &hints, &results)) != 0) {
        printf("Error: Hostname could not be resolved.\n");
        return -1;
    }

    for (struct addrinfo* iter = results; iter != null; iter = iter -> ai_next) {
        getnameinfo(rp -> ai_next)
    }
    connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)); 
        
    return 0;
}
