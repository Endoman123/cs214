#ifndef banking_H
#define banking_H

typedef struct _node{
	char* name;
	double value;
	int serving;
	struct _node* next;
} node;

typedef struct _acceptParams{
	int sfd;
	struct sockaddr* addr;
	socklen_t* slen;
} acceptParams;

typedef struct _sfdPill{
	int sfd;
} sfdPill;

#endif
