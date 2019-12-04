#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/time.h>
#include <signal.h>
#include <semaphore.h>
#include "banking.h"

node* accounts;
pthread_t tids[256];
int sockets[256];
int* connections;
int* killFlag;
sem_t db_lock;

void traverse();

void handle_sigINT(int signum){
	*killFlag = 1;
}

void alarm_handler(int signum){
	sem_wait(&db_lock);
	traverse();
	sem_post(&db_lock);
}

//LL functions
void traverse(){
	if(accounts == NULL){
		printf("No accounts.\n");
		return;
	}
	node* head = accounts;
	while(head != NULL){
		char* serve = "";
		if(head->serving == 1){
			serve = "IN SERVICE";
		} else{
			//serve = "NOT IN SERVICE";
		}
		printf("%s\t%lf\t%s\n", head->name, head->value, serve);
		head = head->next;
	}
	return;
}

void append(node* addend){
	if(addend == NULL){
		printf("Error: NULL pointer given to append\n");
		return;
	} else if(accounts == NULL){
		accounts = addend;
		return;
	}

	node* ptr = accounts;
	while(ptr->next != NULL){
		ptr = ptr->next;
	}
	ptr->next = addend;
	return;
}

node* getAccount(char* name){
	if(accounts == NULL){
		//printf("Error: No accounts created yet\n");
		return NULL;
	}
	if(name == NULL){
		printf("Error: Account name is NULL\n");
		return NULL;
	}
	node* ptr = accounts;
	while(ptr != NULL){
		if((strcmp(ptr->name, name)) == 0){
			return ptr;
		} else{
			ptr = ptr->next;
		}
	}
	//printf("Error: No account under the given name\n");
	return NULL;
}

//Command functions
void create(char* name, int sfd){
	node* served = getAccount(name);
	char* response = "";
	if(served != NULL){
		response = "Error: Already an account under that name.\n";
		send(sfd, response, strlen(response), 0);
		return;
	} else{
		node* new = (node*)malloc(sizeof(node));
		new->name = (char*)malloc(sizeof(char)*256);
		strcpy(new->name, name);
		new->value = 0;
		new->serving = 0;
		new->next=NULL;
		append(new);
		response = "Account created.\n";
		send(sfd, response, strlen(response), 0);
		return;
	}
}

node* serve(char* name, int sfd){
	node* served = getAccount(name);
	char* response = "";
	if(served == NULL){
		response = "Error: No account under that name.\n";
		send(sfd, response, strlen(response), 0);
		return NULL;
	}
	served->serving = 1;
	response = "Now serving.\n";
	send(sfd, response, strlen(response), 0);
	return served;
}

void deposit(char* name, double value, int sfd){
	node* served = getAccount(name);
	char* response = "";
	if(served == NULL){
		response = "Error: No account under that name.\n";
		send(sfd, response, strlen(response), 0);
		return;
	}
	if(value < 0){
		response = "Error: Can't deposit negative amount.\n";
		send(sfd, response, strlen(response), 0);
		return;
	}
	served->value = served->value + value;
	response = "Account balance updated.\n";
	send(sfd, response, strlen(response), 0);
	return;
}

void withdraw(char* name, double value, int sfd){
	node* served = getAccount(name);
	char* response = "";
	if(served == NULL){
		response = "Error: No account under that name.\n";
		send(sfd, response, strlen(response), 0);
		return;
	}
	if(value < 0){
		response = "Error: Can't withdraw negative amount.\n";
		send(sfd, response, strlen(response), 0);
		return;
	}
	if(served->value >= value){
		served->value = served->value - value;
		response = "Account balance updated.\n";
		send(sfd, response, strlen(response), 0);
	} else{
		response = "Error: Can't withdraw more than your balance.\n";
		send(sfd, response, strlen(response), 0);
	}
	return;
}

void query(char* name, int sfd){
	node* served = getAccount(name);
	char response[255];
	int len = 0;
	if(served == NULL){
		snprintf(response, 255, "Error: No account under that name.\n");
		send(sfd, response, strlen(response), 0);
		return;
	}
	len = snprintf(response, 255, "Account balance: %lf.\n", served->value);
	response[len] = '\0';
	send(sfd, response, strlen(response), 0);
	return;
}

void end(char* name, int sfd){
	node* served = getAccount(name);
	char* response = "";
	if(served == NULL){
		response = "Error: No account under that name.\n";
		send(sfd, response, strlen(response), 0);
		return;
	}
	served->serving = 0;
	response = "No longer serving.\n";
	send(sfd, response, strlen(response), 0);
	return;
}

int isServing(char* name){
	node* served = getAccount(name);
	if(served == NULL){
		//printf("Error: No account under that name.\n");
		//return no problem, error will get caught in later serve call
		return -1; 
	}
	if(served->serving == 1){
		return -1;
	}
	return 0;
}

void listener(void* socket_fd){
	//cast args back to actual type
	sfdPill* sfdP = (sfdPill*)socket_fd;
	int sfd = sfdP->sfd;
	
	//init comm. varaibles and send confirmation of connection	
	int msgLength = 0;
	char buffer[1024] = {0};
	char* confirm = "Connected to the server.\n";

	//send and receive confirmations
	send(sfd, confirm, strlen(confirm), 0);
	msgLength = recv(sfd, buffer, 1024, 0);
	buffer[msgLength] = '\0';
	printf("%s", buffer);

	//process input
	node* curr = NULL;
	int gServe = 0;
	while(*killFlag == 0){
		msgLength = recv(sfd, buffer, 1024, 0);
		//get rid of newline
		msgLength = msgLength-1;
		buffer[msgLength] = '\0';
		//printf("%s\n", buffer);
		char cmd[256] = {0};
		char arg[256] = {0};
		int i = 0;
		for(i = 0; i < msgLength; i++){
			if(buffer[i] == ' '){
				break;
			}
			cmd[i] = buffer[i];
		}
		cmd[i] = '\0';
		
		if(i != msgLength){
			int j = 0;
			int k = 0;
			for(j = i+1; j < msgLength; j++){
				arg[k] = buffer[j];
				k++;
			}
			arg[k] = '\0';
		}

		//check commands
		sem_wait(&db_lock);
		char* response = "";
		if((strcmp(cmd, "create")) == 0){
			if(gServe == 1){
				response = "Error: You must end your current service before you create an account.\n";
				send(sfd, response, strlen(response), 0);
			} else{
				create(arg, sfd);
			}
		} else if((strcmp(cmd, "serve")) == 0){
			if(gServe == 1){
				response = "Error: You must end your current service before you serve again.\n";
				send(sfd, response, strlen(response), 0);
			} else if(isServing(arg) == -1){
				response = "Error: Account already being served or not found.\n";
				send(sfd, response, strlen(response), 0);
			} else{
				gServe = 1;
				curr = serve(arg, sfd);
			}
		} else if((strcmp(cmd, "withdraw")) == 0){
			if(gServe == 0){
				response = "Error: You must serve an account to take action to it.\n";
				send(sfd, response, strlen(response), 0);
			} else{
				withdraw(curr->name, atof(arg), sfd);
			}
		} else if((strcmp(cmd, "deposit")) == 0){
			if(gServe == 0){
				response = "Error: You must serve an account to take action to it.\n";
				send(sfd, response, strlen(response), 0);
			} else{
				deposit(curr->name, atof(arg), sfd);
			}
		} else if((strcmp(cmd, "query")) == 0){
			if(gServe == 0){
				response = "Error: You must serve an account to take action to it.\n";
				send(sfd, response, strlen(response), 0);
			} else{
				query(curr->name, sfd);
			}
		} else if((strcmp(cmd, "end")) == 0){
			if(gServe == 0){
				response = "Error: You must serve an account to take action to it.\n";
				send(sfd, response, strlen(response), 0);
			} else{
				end(curr->name, sfd);
				curr = NULL;
				gServe = 0;
			}
		} else if((strcmp(cmd, "quit")) == 0){
			if(gServe == 1){
				response = "Error: You must exit your current session to quit.\n";
				send(sfd, response, strlen(response), 0);
			} else{
				response = "-2";
				send(sfd, response, strlen(response), 0);
				close(sfd);
				printf("A client has disconnected.\n");
				sem_post(&db_lock);
				return;
			}
		}
		sem_post(&db_lock);
	}
	return;
}

void acceptor(void* args){
	acceptParams* acceptArgs = (acceptParams*)args;

	//accept the client
	int new_client = 0;
	while(*killFlag == 0){
		if((new_client = accept(acceptArgs->sfd, acceptArgs->addr, acceptArgs->slen)) < 0){
			perror("Accept");
			exit(EXIT_FAILURE);
		}

		sockets[*connections-1] = new_client;
	
		//init the args for listener
		sfdPill* sfdP = (sfdPill*)malloc(sizeof(sfdPill));
		sfdP->sfd = new_client;
	
		//set up communication between server and accepted client
		int pthread_err = 0;
		if((pthread_err = pthread_create(&tids[*connections], NULL, (void*)listener, (void*)sfdP)) != 0){
			perror("Listener");
			exit(EXIT_FAILURE);
		}
		*connections = (*connections)+1;
	}
	return;
}

void sessionAccept(void* args){
	//create and start diagonstic timer
	struct sigaction alarm;
	struct itimerval timer;
	memset(&alarm, 0, sizeof(alarm));
	alarm.sa_handler = &alarm_handler;
	sigaction(SIGALRM, &alarm, NULL);
	timer.it_value.tv_sec = 15;
	timer.it_value.tv_usec = 15;
	timer.it_interval.tv_sec = 15;
	timer.it_interval.tv_usec = 15;
	setitimer(ITIMER_REAL, &timer, NULL);

	//create and init ctrl-c handler
	struct sigaction killer;
	memset(&killer, 0, sizeof(killer));
	killer.sa_handler = &handle_sigINT;
	sigaction(SIGINT, &killer, NULL);

	//spawn a thread to accept every new connection
	int new_accept = 0;
	if((new_accept = pthread_create(&tids[*connections], NULL, (void*)acceptor, (void*) args)) != 0){
		perror("Acceptor thread creation");
		exit(EXIT_FAILURE);
	} else{
		*connections = (*connections)+1;
	}

	while(*killFlag == 0);
	//lock db
	sem_wait(&db_lock);
	//stop timer
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 0;
	//close all connections
	int i = 0;
	char* msg = "-1";
	for(i = 0; i < 255; i++){
		if(sockets[i] == -2){
			break;		
		}
		send(sockets[i], msg, strlen(msg), 0);
		close(sockets[i]);
	}
	//join all threads
	int j = 0;
	for(j = 0; j < *connections; j++){
		pthread_detach(tids[j]);
	}
	//finish
	exit(0);	
}

int main(int argc, char** argv){
	if(argc != 2){
		printf("Error: Only port number required!\n");
		return -1;
	}

	//init globals
	connections = (int*)malloc(sizeof(int));
	*connections = 0;
	killFlag = (int*)malloc(sizeof(int));
	*killFlag = 0;
	accounts = NULL;
	sem_init(&db_lock, 0, 1);
	int i = 0;
	for(i = 0; i < 255; i++){
		sockets[i] = -2;
	}

	//init function returns and structs
	int server_fd = 0;
	int new_socket = 0;
	int opt = 1;
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	//create socket
	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
		perror("Socket failed");
		exit(EXIT_FAILURE);
	}

	//set rules for new socket
	if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	//set soccket ruleset
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(atoi(argv[1]));

	//bind socket to given port
	if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) > 0){
		perror("Bind failed");
		exit(EXIT_FAILURE);
	}

	//mark socket as listening
	if(listen(server_fd, 5) < 0){
		perror("Listen");
		exit(EXIT_FAILURE);
	}

	//spawn acceptor thread
	acceptParams* acceptArgs = (acceptParams*)malloc(sizeof(acceptParams));
	acceptArgs->sfd = server_fd;
	acceptArgs->addr = (struct sockaddr*)&address;
	acceptArgs->slen = (socklen_t*)&addrlen;
	pthread_t connector_id = NULL;
	if((new_socket = pthread_create(&connector_id, NULL, (void*)sessionAccept, (void*)acceptArgs)) != 0){
		perror("Accept session thread creation");
		exit(EXIT_FAILURE);
	}
	
	//sleep and let threads run, otherwise join will SIGSEGV
	while(1);

return 0;
}
