/*///////////////////////////////////////////////////////////
*
* FILE:		client.c
* AUTHOR:	Davit Gabrielyan
* PROJECT:	CS 3251 Project 1 - Professor Jun Xu 
* DESCRIPTION:	Network Client Code
* CREDIT:	Adapted from Professor Traynor
*
*////////////////////////////////////////////////////////////

/* Included libraries */

#include <stdio.h>		    /* for printf() and fprintf() */
#include <sys/socket.h>		    /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>		    /* for sockaddr_in and inet_addr() */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

/* Constants */
#define RCVBUFSIZE 512		    /* The receive buffer size */
#define SNDBUFSIZE 512		    /* The send buffer size */
#define REPLYLEN 32

#define SERVER_PORT 5000
#define MAXSIZE 20

void err_n_die(char *fmt, ...);

/* The main function */
int main(int argc, char *argv[])
{

    int clientSock;		    /* socket descriptor */
    struct sockaddr_in serv_addr;   /* server address structure */

    char *accountName;		    /* Account Name  */
    char *servIP;		    /* Server IP address  */
    unsigned short servPort;	    /* Server Port number */
    

    char sndBuf[SNDBUFSIZE];	    /* Send Buffer */
    char rcvBuf[RCVBUFSIZE];	    /* Receive Buffer */
    
    int balance;		    /* Account balance */

    char request_type;
    char *next_account;
    char *amount;

    char *error_msg = "Error occured on the surver. Likely wrong account inputted";


    /* Get the Account Name from the command line */
    if (argc < 5 || argc > 7) 
	   err_n_die("Incorrect number of arguments. The correct format is:\n\t %s <accountName> <serverIP> <serverPort> ...", argv[0]);

    accountName = argv[1];
    memset(&sndBuf, 0, SNDBUFSIZE);
    memset(&rcvBuf, 0, RCVBUFSIZE);

    /* Get the addditional parameters from the command line */
    /*	    FILL IN	*/
    servIP = argv[2];
    servPort = atoi(argv[3]);

    if (argc == 5) {
        request_type = 'B';
    } else if (argc == 6) {
        request_type = 'W';
        amount = argv[5];
    } else if (argc == 7) {
        request_type = 'T';
        amount = argv[6];
        next_account = argv[5];
    }

    /* Create a new TCP socket*/
    /*	    FILL IN	*/
    clientSock = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSock < 0)
        err_n_die("Something went wrong when creating the client socket");

    /* Construct the server address structure */
    /*	    FILL IN	 */
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(servPort);

    if (inet_pton(AF_INET, servIP, &serv_addr.sin_addr) <= 0)
        err_n_die("inet_pton errored for %s please check your IP address", servIP);


    /* Establish connecction to the server */
    /*	    FILL IN	 */
    if (connect(clientSock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
        err_n_die("connection to server failed, please make sure it is up and running");

    
    /* Send the string to the server */
    /*	    FILL IN	 */
    //pack data into the send buffer
    int byte_counter = 1;

    sndBuf[0] = request_type; //specify request type in the first byte

    strncpy(sndBuf + byte_counter, accountName, strlen(accountName));
    byte_counter += strlen(accountName);
    *(sndBuf + byte_counter) = 'X';
    byte_counter++;

    if (request_type == 'W' || request_type == 'T') {
        strncpy(sndBuf + byte_counter, amount, strlen(amount));
        byte_counter += strlen(amount);
        *(sndBuf + byte_counter) = 'X';
        byte_counter++;
    } 
    if (request_type == 'T') {
        strncpy(sndBuf + byte_counter, next_account, strlen(next_account));
        byte_counter += strlen(next_account);
        *(sndBuf + byte_counter) = 'X';
        byte_counter++;
    }

    //printf("packed buffer %s and len: %d\n", sndBuf, (int) strlen(sndBuf));

    send(clientSock, sndBuf, SNDBUFSIZE, 0); // send the packed buffer over

    /* Receive and print response from the server */
    /*	    FILL IN	 */
    recv(clientSock, rcvBuf, RCVBUFSIZE, 0);
    //balance = atoi(rcvBuf);

    if (strlen(rcvBuf) == 0) strncpy(rcvBuf, error_msg, strlen(error_msg));

    printf("\nThe following account was targeted: %s\n", accountName);
    printf("%s\n", rcvBuf);

    return 0;
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






