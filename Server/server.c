#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h> /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h> /* for atoi() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include <stdbool.h> /* for boolean data types */

#define MAXPENDING 5 /* Maximum outstanding connection requests */
#define RCVBUFSIZE 32 /* Size of receive buffer */

void DieWithError(char *errorMessage); /* Error handling function */

int main(int argc, char *argv[])
{
    int servSock; /* Socket descriptor for server */
    int clntSock; /* Socket descriptor for client */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned short echoServPort; /* Server port */
    unsigned int clntLen; /* Length of client address data structure */

    int clientCount = 0;
    bool loginStatus = false;
    bool userOneLoggedIn = false;
    bool userTwoLoggedIn = false;
    char users[2][32]; /* users stored in server */
    char passwords[2][32]; /* passwords stored in server */
    char username[50]; /* username buffer */
    char password[50]; /* password buffer */
    char userOption[2]; /* client menu selection buffer */
    char message[1000]; /* message to send */
    char userMessages[2][1000]; /* user[i] messages */
    char echoMessageStatus[50];
    char receiver[50]; /* receiver of message */


    echoServPort = 8000; /* Define local port */

    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError( "socket () failed") ;

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
    echoServAddr.sin_family = AF_INET; /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort); /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError ( "bind () failed");
    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0)
        DieWithError("listen() failed") ;

    /* Load users and passwords */
    strcpy(users[0], "Alice");
    strcpy(users[1], "Bob");
    strcpy(passwords[0], "12345");
    strcpy(passwords[1], "56789");

    printf("Server Started!\n");
    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        clntLen = sizeof(echoClntAddr);
        /* Wait for a client to connect */
        if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
            DieWithError("accept() failed");
        /* clntSock is connected to a client! */
        printf("Listening on %s:%i\n", inet_ntoa(echoClntAddr.sin_addr), echoServPort);
        clientCount++;
        printf("\nClient %d connected\n", clientCount);

        char echoBuffer[RCVBUFSIZE]; /* Buffer for echo string */
        int echoStringSize; /* Size of echo string */
        int recvMsgSize; /* Size of received message */

        /* Receive menu selections and inputs from client */
        if ((recvMsgSize = recv(clntSock, userOption, 2, 0)) < 0)
            DieWithError("recv() failed") ;
        userOption[recvMsgSize] = '\0';

        /* Send received string and receive again until end of transmission */
        while (recvMsgSize > 0) /* zero indicates end of transmission */
        {
            if(loginStatus) {
                printf("Logged In Receiving User Option...\n");
                if ((recvMsgSize = recv(clntSock, userOption, 2, 0)) < 0)
                    DieWithError("recv() failed") ;
                userOption[recvMsgSize] = '\0';
            }

            if(strcmp(userOption, "0") == 0) {
                if((recvMsgSize = recv(clntSock, username, RCVBUFSIZE, 0)) < 0)
                    DieWithError("recv() failed");
                username[recvMsgSize] = '\0';
                printf("Login Username is %s\n", username);
                if((recvMsgSize = recv(clntSock, password, RCVBUFSIZE, 0)) < 0)
                    DieWithError("recv() failed");
                password[recvMsgSize] = '\0';
                printf("Login Password is %s\n", password);
                int i = 0;
                for(i = 0; i < 2; i++) {
                    if((strcmp(username, users[i]) == 0) && (strcmp(password, passwords[i]) == 0)){
                        strcpy(echoBuffer, "login success\n");
                        if(i == 0)
                            userOneLoggedIn = true;
                        if(i == 1)
                            userTwoLoggedIn = true;
                    }
                }
                printf("%s", echoBuffer);
                echoStringSize = strlen(echoBuffer);
                if(send(clntSock, echoBuffer, echoStringSize, 0) != echoStringSize)
                    DieWithError("send() failed: login_Status");
                else {
                    memset(echoBuffer, 0, RCVBUFSIZE);
                    // printf("EchoBuffer: %s\n", echoBuffer);
                    memset(userOption, 0, 2);
                    // printf("UserOption: %s\n", userOption);
                    // memset(username, 0, 50);
                    loginStatus = true;
                }
            }
            else if(strcmp(userOption, "1") == 0) {
                printf("Return user list\n");
                // printf("There are %lu users\n", (sizeof(users)/RCVBUFSIZE));
                strcpy(echoBuffer, "There are 2 users\n");
                int i;
                for( i = 0; i < 2; i++) {
                    strcat(echoBuffer, users[i]);
                    strcat(echoBuffer, "\n");
                }
                echoStringSize = strlen(echoBuffer);
                if(send(clntSock, echoBuffer, echoStringSize, 0) != echoStringSize)
                    DieWithError("send() failed: users_lists");
                memset(echoBuffer, 0, RCVBUFSIZE);
                memset(userOption, 0, 2);
            }
            else if(strcmp(userOption, "2") == 0) {
                if((recvMsgSize = recv(clntSock, receiver, RCVBUFSIZE, 0)) < 0)
                    DieWithError("recv() failed");
                receiver[recvMsgSize] = '\0';
                printf("A message to %s\n", receiver);
                if((recvMsgSize = recv(clntSock, message, RCVBUFSIZE, 0)) < 0)
                    DieWithError("recv() failed");
                message[recvMsgSize] = '\0';
                printf("Message is: %s\n", message);
                if(strcmp(receiver, "Alice") == 0)
                    strcpy(userMessages[0], message);
                else if(strcmp(receiver, "Bob") == 0)
                    strcpy(userMessages[1], message);
                strcpy(echoBuffer, "message sent successfully\n");
                echoStringSize = strlen(echoBuffer);
                if(send(clntSock, echoBuffer, echoStringSize, 0) != echoStringSize)
                    DieWithError("send() failed: message delivery status");
                memset(echoBuffer, 0, RCVBUFSIZE);
                memset(userOption, 0, 2);
            }
            else if(strcmp(userOption, "3") == 0) {
                printf("Send back %s's message\n", username);
                strcpy(echoBuffer, "You have ");
                if(strcmp(receiver, username) == 0) {
                    if(strcmp(username, "Alice")) {
                        strcat(echoBuffer, "1 message(s)!\n");
                        strcat(echoBuffer, message);
                    }
                    else if(strcmp(username, "Bob")) {
                        strcat(echoBuffer, "1 message(s)!\n");
                        strcat(echoBuffer, message);
                    }
                }
                else
                    strcat(echoBuffer, "0 message(s)!\n");

                echoStringSize = strlen(echoBuffer);
                if(send(clntSock, echoBuffer, echoStringSize, 0) != echoStringSize)
                    DieWithError("send() failed: message delivery status");
                memset(echoBuffer, 0, RCVBUFSIZE);
                memset(userOption, 0, 2);
            }
            else if(strcmp(userOption, "4") == 0){
                printf("Client disconnected!\n");
                strcpy(echoBuffer, "Client disconnected!\n");
                echoStringSize = strlen(echoBuffer);
                if(send(clntSock, echoBuffer, echoStringSize, 0) != echoStringSize)
                    DieWithError("send() failed: message delivery status");
                // close(clntSock);
                loginStatus = false;
                memset(userOption, 0, 2);
                memset(username, 0, 50);
                memset(password, 0, 50);
                memset(echoBuffer, 0, RCVBUFSIZE);
                
                /* Stop waiting on client input */
                recvMsgSize = -1;
                // printf("%d", recvMsgSize);
                close(clntSock);
            }
            else if(strcmp(userOption, "5") == 0) {
                printf("Client disconnected!\n");
                strcpy(echoBuffer, "Client disconnected!\n");
                echoStringSize = strlen(echoBuffer);
                if(send(clntSock, echoBuffer, echoStringSize, 0) != echoStringSize)
                    DieWithError("send() failed: message delivery status");
                loginStatus = false;
                memset(userOption, 0, 2);
                memset(username, 0, 50);
                memset(echoBuffer, 0, RCVBUFSIZE);

                /* Stop waiting on client input */
                recvMsgSize = -1;
                close(clntSock);
            }
        }
        printf("Waiting for clients to connect...\n");
        close(clntSock);
    }

}
/* NOT REACHED */
