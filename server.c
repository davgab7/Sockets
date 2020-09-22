/*///////////////////////////////////////////////////////////
*
* FILE:		server.c
* AUTHOR:	Davit Gabrielyan
* PROJECT:	CS 3251 Project 1 - Professor Jun Xu
* DESCRIPTION:	Network Server Code
* CREDIT:	Adapted from Professor Traynor
*
*////////////////////////////////////////////////////////////

/*Included libraries*/

#include <stdio.h>	  /* for printf() and fprintf() */
#include <sys/socket.h>	  /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>	  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>	  /* supports all sorts of functionality */
#include <unistd.h>	  /* for close() */
#include <string.h>	  /* support any string ops */
#include <stdarg.h>
#include <errno.h>


#define RCVBUFSIZE 512		/* The receive buffer size */
#define SNDBUFSIZE 512		/* The send buffer size */
#define BUFSIZE 40		/* Your name can be as many as 40 chars*/
#define MAXSIZE 25

typedef struct {
    int mySavings;
    int myChecking;
    int myCD;
    int my401k;
    int my529; 
} Account_Balances;

void err_n_die(char *fmt, ...);
int doStuff(int *balance, int *amount, char *next_account);
void send_to_client(int *clientSock, char *buf);

/* The main function */
int main(int argc, char *argv[])
{

    Account_Balances accounts = {1000, 2000, 3000, 4000, 5000};

    int serverSock;				/* Server Socket */
    int clientSock;				/* Client Socket */
    struct sockaddr_in changeServAddr;		/* Local address */
    struct sockaddr_in changeClntAddr;		/* Client address */
    unsigned short changeServPort;		/* Server port */
    unsigned int clntLen;			/* Length of address data struct */

    char nameBuf[BUFSIZE];			/* Buff to store account name from client */
    int  balance;				/* Place to record account balance result */
    int new_balance;

    char client_addr[MAXSIZE];

    char request_type;
    char amount_string[MAXSIZE];
    int  amount_int = 0;
    char next_account[MAXSIZE];

    char msg[RCVBUFSIZE];

    char *balance_error = "Error: Insufficient Funds!";
    char *success_message = "Transaction Successful! Server updated.";
    char *balance_message = "Balance is: ";
    char *withdraw_error = "Error: too many withdrawals in a minute!";

    char sndBuffer[SNDBUFSIZE];


    /* Create new TCP Socket for incoming requests*/
    /*	    FILL IN	*/
    serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0) err_n_die("socket error");

    /* Construct local address structure*/
    /*	    FILL IN	*/
    bzero(&changeServAddr, sizeof(changeServAddr));
    changeServAddr.sin_family = AF_INET;
    changeServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    changeServAddr.sin_port = htons(atoi(argv[2]));
    
    /* Bind to local address structure */
    /*	    FILL IN	*/
    if ((bind(serverSock, (struct sockaddr *) &changeServAddr, sizeof(changeServAddr))) < 0)
        err_n_die("bind error");

    /* Listen for incoming connections */
    /*	    FILL IN	*/
    if (listen(serverSock, 4) < 0)
        err_n_die("listen error");

    /* Loop server forever*/
    while(1)
    {

    printf("\nWaiting for connections on %s:%s\n", argv[1], argv[2]);
    fflush(stdout);

	/* Accept incoming connection */
	/*	FILL IN	    */
    clntLen = sizeof(changeClntAddr);
    clientSock = accept(serverSock, (struct sockaddr *) &changeClntAddr, &clntLen);
    if (clientSock < 0)
        err_n_die("error with accept");

    inet_ntop(AF_INET, &changeClntAddr, client_addr, MAXSIZE);
    printf("Client connection IP address: %s\n", client_addr);

	/* Extract the account name from the packet, store in nameBuf */
	/* Look up account balance, store in balance */
	/*	FILL IN	    */

    //Init these to 0 every time
    memset(nameBuf, 0, BUFSIZE);
    memset(amount_string, 0, MAXSIZE);
    amount_int = 0;
    memset(next_account, 0, MAXSIZE);
    memset(sndBuffer, 0, SNDBUFSIZE);
    memset(msg, 0, RCVBUFSIZE);

    recv(clientSock, msg, RCVBUFSIZE, 0); // recieving the message from client
    //printf("msg recieved from client: %s\n", msg);

    request_type = msg[0];

    int byte_counter = 1;
    char *msg_tmp = msg + 1;

    char *x = strchr(msg_tmp, 'X');
    int index_of_X = (int) (x - msg_tmp);

    strncpy(nameBuf, msg + byte_counter, index_of_X);
    byte_counter += strlen(nameBuf);
    byte_counter++;

    if (request_type == 'W' || request_type == 'T') {
        x = strchr(x + 1,'X');
        index_of_X = (int) (x - msg_tmp);

        strncpy(amount_string, msg + byte_counter, (index_of_X - byte_counter) + 1);
        amount_int = atoi(amount_string);
        byte_counter += strlen(amount_string);
        byte_counter++;
        //printf("amount: %s and len: %d\n", amount_string, (int) strlen(amount_string));


    } 
    if (request_type == 'T') {
        x = strchr(x + 1,'X');
        index_of_X = (int) (x - msg_tmp);

        strncpy(next_account, msg + byte_counter, (index_of_X - byte_counter) + 1);
        byte_counter += strlen(next_account);
        //printf("next account: %s and len: %d\n", next_account, (int) strlen(next_account));
    }

	/* Return account balance to client */
	/*	FILL IN	    */

    if (strcmp(nameBuf, "mySavings") == 0) {
        if (doStuff(&accounts.mySavings, &amount_int, next_account) < 0) {send_to_client(&clientSock, balance_error); continue;}
        balance = accounts.mySavings;
    } else if (strcmp(nameBuf, "myChecking") == 0) {
        if (doStuff(&accounts.myChecking, &amount_int, next_account) < 0) {send_to_client(&clientSock, balance_error); continue;}
        balance = accounts.myChecking;
    } else if (strcmp(nameBuf, "myCD") == 0) {
        if (doStuff(&accounts.myCD, &amount_int, next_account) < 0) {send_to_client(&clientSock, balance_error); continue;}
        balance = accounts.myCD;
    } else if (strcmp(nameBuf, "my401k") == 0) {
        if (doStuff(&accounts.my401k, &amount_int, next_account) < 0) {send_to_client(&clientSock, balance_error); continue;}
        balance = accounts.my401k;
    } else if (strcmp(nameBuf, "my529") == 0) {
        if (doStuff(&accounts.my529, &amount_int, next_account) < 0) {send_to_client(&clientSock, balance_error); continue;}
        balance = accounts.my529;
    } else {
        err_n_die("Unknown account was passed in %s", nameBuf);
    }

    if (request_type == 'T') {
        if (strcmp(next_account, "mySavings") == 0) {
            accounts.mySavings += amount_int;
            new_balance = accounts.mySavings;
        } else if (strcmp(next_account, "myChecking") == 0) {
            accounts.myChecking += amount_int;
            new_balance = accounts.myChecking;
        } else if (strcmp(next_account, "myCD") == 0) {
            accounts.myCD += amount_int;
            new_balance = accounts.myCD;
        } else if (strcmp(next_account, "my401k") == 0) {
            accounts.my401k += amount_int;
            new_balance = accounts.my401k;
        } else if (strcmp(next_account, "my529") == 0) {
            accounts.my529 += amount_int;
            new_balance = accounts.my529;
        } else {
            err_n_die("Unknown account was passed in %s", nameBuf);
        } 
    }

    //printf("balance is now: %d\n", balance); printf("New balance is now: %d\n", new_balance);exit(1);

    char balance_string[MAXSIZE];
    snprintf(balance_string, MAXSIZE, "%d", balance); // convert int balance to a string to send over

    if (request_type == 'B') {
        strncpy(sndBuffer, balance_message, strlen(balance_message));
        strncpy(sndBuffer+strlen(balance_message), balance_string, strlen(balance_string));
        send_to_client(&clientSock, sndBuffer);
    } else {
        send_to_client(&clientSock, success_message);
    }

    }

}

int doStuff(int *balance, int *amount, char *next_account) {
    if (*amount == 0) return 0;

    if (*balance < *amount) {
        return -1;
    }

    *balance -= *amount;
    return 0;
}

void send_to_client(int *clientSock, char *buf) {
    send(*clientSock, buf, strlen(buf), 0);
}

void err_n_die(char *fmt, ...) {
    va_list args;
    int i, errno_save = 0;

    errno_save = errno;

    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\n");
    fflush(stdout);

    if (errno_save != 0) {
        fprintf(stdout, "errno = %d : %s\n", errno_save, strerror(errno_save));
        fprintf(stdout, "\n");
        fflush(stdout); 
    }

    va_end(args);
    exit(1);
}

