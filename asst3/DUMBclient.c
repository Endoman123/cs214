#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>

int main(int argc, char **argv) {
    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
 
    // Specify server ip and port
    struct sockaddr_in servAddr;
 
    servAddr.sin_family = AF_INET; // Use IPv4
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Set IP
    servAddr.sin_port = htons(4096); // Set port
    connect(sock, (struct sockaddr *)&servAddr, sizeof(servAddr));

    // Read from server
    char buffer[40];
    read(sock, buffer, sizeof(buffer) - 1);

    printf("Message from server: %s\n", buffer);

    // Close
    close(sock);

    return 0;
}
