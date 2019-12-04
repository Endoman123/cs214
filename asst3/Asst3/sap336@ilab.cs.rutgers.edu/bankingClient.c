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
#include "banking.h"

pthread_t tids[2];

int isDigitString(char* arg){
	if(arg == NULL){
		printf("Error: NULL input.\n");
		return -1;
	}
	int i = 0;
	int decFlag = 0;
	for(i = 0; i < strlen(arg); i++){
		if(arg[i] == '.'){
			decFlag++;
		}
		if((isdigit(arg[i]) == 0) && ((arg[i] != '.') || (decFlag > 1))){
			return -1;
		}
	}
	return 0;
}
void commandInput(void* args){
	sfdPill* sfdP = (sfdPill*)args;
	int sfd = sfdP->sfd;

	char* confirm = "Connected to a new client.\n";
	send(sfd, confirm, strlen(confirm), 0);
	
	char buffer[1024] = {0};
	int msgLength = 0;
	while(1) {
		sleep(2);
		msgLength = read(0, buffer, 1024);
		//ignore newline
		msgLength = msgLength - 1;
		
		char cmd[256] = {0};
		char arg[256] = {0};
		char fullMsg[256] = {0};
		int i = 0;
		for(i = 0; i < msgLength; i++){
			if(buffer[i] == ' '){
				break;
			}
			cmd[i] = buffer[i];
			fullMsg[i] = buffer[i];
		}
		cmd[i+1] = '\0';
		
		int j = 0;
		if(i != msgLength){
			int k = 0;
			fullMsg[i] = ' ';
			for(j = i+1; j < msgLength; j++){
				arg[k] = buffer[j];
				fullMsg[j] = buffer[j];
				k++;
			}
			arg[k+1] = '\0';
		}

		//check commands
		char* response = "";
		if((strcmp(cmd, "create")) == 0){
			if(i == msgLength){
				response = "Error: You must provide an argument for this command.\n";
				printf("%s", response);
			} else{
				send(sfd, fullMsg, strlen(fullMsg)+1, 0);
			}
		} else if((strcmp(cmd, "serve")) == 0){
			if(i == msgLength){
				response = "Error: You must provide an argument for this command.\n";
				printf("%s", response);
			} else{
				send(sfd, fullMsg, strlen(fullMsg)+1, 0);
			}
		} else if((strcmp(cmd, "withdraw")) == 0){
			if(i == msgLength){
				response = "Error: You must provide an argument for this command.\n";
				printf("%s", response);
			} else if(isDigitString(arg) == -1){
				response = "Error: Argument invalid.\n";
				printf("%s", response);
			} else{
				send(sfd, fullMsg, strlen(fullMsg)+1, 0);
			}
		} else if((strcmp(cmd, "deposit")) == 0){
			if(i == msgLength){
				response = "Error: You must provide an argument for this command.\n";
				printf("%s", response);
			} else if(isDigitString(arg) == -1){
				response = "Error: Argument invalid.\n";
				printf("%s", response);
			} else{
				send(sfd, fullMsg, strlen(fullMsg)+1, 0);
			}
		} else if((strcmp(cmd, "query")) == 0){
			if(j != 0){
				response = "Error: Query does not take an argument.\n";
				printf("%s", response);
			} else if(buffer[i] == ' '){
				response = "Error: No spaces after commands please.\n";
				printf("%s", response);
			} else{
				send(sfd, fullMsg, strlen(fullMsg)+1, 0);
			}
		} else if((strcmp(cmd, "end")) == 0){
			if(j != 0){
				response = "Error: End does not take an argument.\n";
				printf("%s", response);
			} else if(buffer[i] == ' '){
				response = "Error: No spaces after commands please.\n";
				printf("%s", response);
			} else{
				send(sfd, fullMsg, strlen(fullMsg)+1, 0);
			}
		} else if((strcmp(cmd, "quit")) == 0){
			if(j != 0){
				response = "Error: Quit does not take an argument.\n";
				printf("%s", response);
			} else if(buffer[i] == ' '){
				response = "Error: No spaces after commands please.\n";
				printf("%s", response);
			} else{
				send(sfd, fullMsg, strlen(fullMsg)+1, 0);
			}
		} else{
			response = "Error: Unrecognized command.\n";
			printf("%s", response);
		}
	}
}

void reader(void* args){
	sfdPill* sfdP = (sfdPill*)args;
	int sfd = sfdP->sfd;

	int msgLength = 0;
	char buffer[1024] = {0};
	while(1){
		msgLength = recv(sfd, buffer, 1024, 0);
		buffer[msgLength] = '\0';
		if(buffer[0] == '-' && buffer[1] == '1'){
			printf("The server has shutdown. Ending client.\n");
			sleep(1);
			exit(0);
		} else if(buffer[0] == '-' && buffer[1] == '2'){
			printf("Server has acknowledged quit request. Connection has ended.\n");
			sleep(1);
			exit(0);
		} else{
			printf("%s", buffer);
		}
	}
}


int main(int argc, char** argv){
	if(argc != 3){
		printf("Error: Incorrect number of arguments. Need server address and port.\n");
		return 0;
	}

	//struct sockaddr_in address;
	struct sockaddr_in serv_addr;
	struct addrinfo hints;
	struct addrinfo *results, *rp;
	int sock = 0;
	char host[256];
	
	//create socket
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("Error: Socket creation error.\n");
		return -1;
	}

	//init serv_addr
	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[2]));

	//init hints for hostname conversion
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	
	//resolve hostname
	int addr_error;
	if((addr_error = getaddrinfo(argv[1], NULL, &hints, &results)) != 0){
		printf("Error: Something went wrong with host name conversion.\n");
		fprintf(stderr, "%s\n", gai_strerror(addr_error));
		return -1;
	}

	for(rp = results; rp != NULL; rp = rp->ai_next){
		getnameinfo(rp->ai_addr, rp->ai_addrlen, host, sizeof(host), NULL, 0, NI_NUMERICHOST);
		inet_aton(host, &serv_addr.sin_addr);
	}

	//attempt to connect to server
	while(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
		printf("Error: Connection failed.\n");
		sleep(3);
	}

	sfdPill* sfdP = (sfdPill*)malloc(sizeof(sfdPill));
	sfdP->sfd = sock;
	
	int err = 0;
	if((err = pthread_create(&tids[0], NULL, (void*)commandInput, (void*)sfdP)) != 0){
		printf("Error: pthread_creation.\n");
		return -1;
	}
	
	if((err = pthread_create(&tids[1], NULL, (void*)reader, (void*)sfdP)) != 0){
		printf("Error: pthread_creation.\n");
		return -1;
	}
	
	int i = 0;
	for(i = 0; i < 2; i++){
		if((err = pthread_join(tids[i], NULL)) != 0){
			printf("Error: Joining.\n");
			return -1;
		}
	}

    return 0;
}
