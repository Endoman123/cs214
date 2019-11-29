#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    //The user should input an ip address and a port.
    if (argc != 3) {
        printf("Error: Incorrect number of arguments for the client.\n");
        return 1;
    }

    char* ip_address = argv[1];
    int port = atoi(argv[2]); //TODO: *breaths in* SIZE CHECK. 
    
    if (port >= 4096) {
        printf("Error: The port should be above 4096\n");
        return 1;
    }

    //Create the client socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);  
    
    struct sockaddr_in serv_addr;

    serv_addr.sin_family = AF_INET;     
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 127.0.0.1 binds to the local address
    serv_addr.sin_port = htons(port);
    connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)); 
        
    return 0;
}
