#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h> /* for sockaddr_in and inet_addr() */
#include <stdlib.h> /* for atoi() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include <stdbool.h> /* for boolean data types */

#define MAXPENDING 5 /* Maximum outstanding connection requests */
#define RCVBUFSIZE 32 /* Size of receive buffer */

void DieWithError(char *errorMessage); /* Error handling function */

void printMenu(void)
{
    printf("----------------------------------------\n");
    printf("Command:\n");
    printf("0. Connect to the server\n");
    printf("1. Get the user list\n");
    printf("2. Send a message\n");
    printf("3. Get my messages\n");
    printf("4. Initial a chat with my friend\n");
    printf("5. Chat with my friend\n");
}

int main(int argc, char *argv[])
{
    int sock; /* Socket descriptor */
    int chatSock; /* Chat Socket descriptor */
    int clntSock;
    struct sockaddr_in echoServAddr; /* Echo server address */
    struct sockaddr_in echoClntAddr; /* Client address */ 
    unsigned short echoServPort; /* Echo server port */
    char *servlP; /* Server IP address (dotted quad) */
    char c[20]; /* Server IP buffer */
    char *echoString; /* String to send to echo server */
    char echoBuffer[RCVBUFSIZE]; /* Buffer for echo string */
    unsigned int echoStringLen; /* Length of string to echo */
    int bytesRcvd, totalBytesRcvd; /* Bytes read in single recv() and total bytes read */
    unsigned int clntLen; /* Length of client address data structure */
    int recvMsgSize; /* Size of received message */

    char clientOption[1];
    char clientUsername[30];
    char clientPassword[30];
    char receiver[30];
    char receiverMsg[1000];
    char receiverMsg2[1000];
    bool connectionStatus = false;
    bool loginStatus = false;
    bool run_program = true;
    bool connectChat = false;
    char message[1000];
    char message2[1000];
    char clientUsername2[30];

    while(run_program) {
    printMenu();
    printf("Your option (enter a number): \n");
    scanf("%s", clientOption);
    // printf("\n%s\n", clientOption);
    if(connectionStatus) {
        // printf("\nSend clientOption via Connection Status:%s\n", clientOption);
        echoStringLen = strlen(clientOption);
        if (send(sock, clientOption, 1, 0) != echoStringLen)
            DieWithError("send() sent a different number of bytes than expected");
    }
    if(strcmp(clientOption, "0") == 0) {
        while(!connectionStatus){
            printf("Please enter the IP address: ");
            scanf("%s", c);
            servlP = c;
            // printf("\n%s", servIP);
            printf("Please enter the port number: ");
            scanf("%hu", &echoServPort);
            // printf("\n%hu", echoServPort);
            if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
                DieWithError(" socket() failed");

            memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
            echoServAddr.sin_family = AF_INET; /* Internet address family */
            echoServAddr.sin_addr.s_addr = inet_addr(servlP); /* Server IP address */
            echoServAddr.sin_port = htons(echoServPort); /* Server port */

            if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0) 
                DieWithError(" connect () failed");
            else {
                connectionStatus = true;
                if(send(sock, clientOption, 1, 0) != 1)
                    DieWithError("send() clientOption: connection sent a different number of bytes than expected");
                memset(clientOption, 0, 1);
                memset(c, 0, 20);
                // printf("\nEnd option 0:%s\n", clientOption);
            }

        }
        printf("Connecting....");
        printf("\nConnected!");
        printf("\nWelcome. Please log in.");
        printf("\nUsername: ");
        scanf("%s", clientUsername);
        echoStringLen = strlen(clientUsername);

        if(send(sock, clientUsername, echoStringLen, 0) != echoStringLen)
            DieWithError("send() : username sent a different number of bytes than expected");

        printf("Password: ");
        scanf("%s", clientPassword);
        echoStringLen = strlen(clientPassword);
        if (send(sock, clientPassword, echoStringLen, 0) != echoStringLen)
            DieWithError("send() : password sent a different number of bytes than expected");

        if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
            DieWithError("recv() failed or connection closed prematurely");
        // echoBuffer[bytesRcvd] = '\0'; /* Terminate the string! */
        printf("%s", echoBuffer); /* Print the echo buffer */
        
        if(strcmp(echoBuffer, "login success\n") == 0) {
            loginStatus = true;
            memset(clientOption, 0, 1);
        }
        printf("\nEnd option 0:%s\n", clientOption);
    }
    if(strcmp(clientOption, "1") == 0 ){
        // if(send(sock, clientOption, 1, 0) != 1)
        //     DieWithError("send() clientOption: connection sent a different number of bytes than expected");

        if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
            DieWithError("recv() failed or connection closed prematurely");
        // echoBuffer[bytesRcvd] = '\0'; /* Terminate the string! */
        printf("%s", echoBuffer); /* Print the echo buffer */
        memset(clientOption, 0, 1);
        printf("\nEnd option 0:%s\n", clientOption);
    }

    if(strcmp(clientOption, "2") == 0) {
        // if(send(sock, clientOption, 1, 0) != 1)
        //     DieWithError("send() clientOption: connection sent a different number of bytes than expected");

        printf("Please enter the user name: ");
        scanf("%s", receiver);
        echoStringLen = strlen(receiver);
        if(send(sock, receiver, echoStringLen, 0) != echoStringLen)
            DieWithError("send() : receiver sent a different number of bytes than expected");
        
        printf("Please enter the message: ");
        scanf("%s", receiverMsg);
        fgets(receiverMsg2, 1000, stdin);
        strcat(receiverMsg, receiverMsg2);
        echoStringLen = strlen(receiverMsg);
        if(send(sock, receiverMsg, echoStringLen, 0) != echoStringLen)
            DieWithError("send() : username sent a different number of bytes than expected");

        if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
            DieWithError("recv() failed or connection closed prematurely");
        // echoBuffer[bytesRcvd] = '\0'; /* Terminate the string! */
        printf("%s", echoBuffer); /* Print the echo buffer */   
        memset(clientOption, 0, 1); 
        memset(receiverMsg, 0, 1000);
        // printf("\nEnd option 0:%s\n", clientOption);       
    }

    if(strcmp(clientOption, "3") == 0) {
        // if(send(sock, clientOption, 1, 0) != 1)
        //     DieWithError("send() clientOption: connection sent a different number of bytes than expected");

        if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
            DieWithError("recv() failed or connection closed prematurely");
        // echoBuffer[bytesRcvd] = '\0'; /* Terminate the string! */
        printf("%s", echoBuffer); /* Print the echo buffer */
        memset(clientOption, 0, 1);
        // printf("\nEnd option 0:%s\n", clientOption);
    }

    if(strcmp(clientOption, "4") == 0) {
        // if(send(sock, clientOption, 1, 0) != 1)
        //     DieWithError("send() clientOption: connection sent a different number of bytes than expected");
        memset(clientOption, 1, 0);
        if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
            DieWithError("recv() failed or connection closed prematurely");
        echoBuffer[bytesRcvd] = '\0'; /* Terminate the string! */
        if(strcmp(echoBuffer, "Client disconnected!\n") == 0) {    
            close(sock);
            printf("Server Socket Connection Closed\n");
        }

        strcpy(servlP, "127.0.0.1");
        // printf("%s\n", servlP);
        /* Construct local address structure */
        // memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */

        printf("-------------disconnect with the server-----------\n");
        printf("Please enter the port number you want to listen on: ");
        scanf("%hu", &echoServPort);

        // echoServAddr.sin_family = AF_INET; /* Internet address family */
        // echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
        // echoServAddr.sin_port = htons(echoServPort); /* Local port */

        if ((chatSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
            DieWithError(" socket() failed: chatSock");
        // else
        //     printf("socket success!\n");

        echoServAddr.sin_port = htons(echoServPort); /* Server port */

        /* bind Chat Sock to local address for other chat recipient */ 
        if(bind(chatSock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) < 0)
            DieWithError(" bind() failed: chatSock");
        // else
        //     printf("bind() success\n");

        if(listen(chatSock, MAXPENDING) < 0)
            DieWithError(" listen() failed: chatSock");
        else
            printf("I am Listening on %s:%hu\n", servlP, echoServPort);

        /* Set the size of the in-out parameter */
        clntLen = sizeof(echoClntAddr);

        // printf("I am user %s", clientUsername);
        if((clntSock = accept(chatSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
            DieWithError(" accept() failed: chatSock");

        if((recvMsgSize = recv(clntSock, clientUsername2, 30, 0)) < 0)
            DieWithError("recv() failed");
        clientUsername2[recvMsgSize] = '\0';
        printf("%s is connected\n", clientUsername2);
        if(send(clntSock, clientUsername, echoStringLen, 0) != echoStringLen)
            DieWithError("send() sent a different number of bytes than expected!");
        connectChat = true;
        while(connectChat) {
            if((recvMsgSize = recv(clntSock, message, 1000, 0)) < 0)
                DieWithError("recv() failed");
            message[recvMsgSize] = '\0';
            printf("(Type 'Bye' to stop the conversation)\n");
            printf("%s: %s", clientUsername2, message);
            if(strcmp(message, "Bye\n") == 0)
                connectChat = false;
            memset(message, 0, 1000);
            printf("%s: ", clientUsername);
            scanf("%s", message);
            fgets(message2, 1000, stdin);
            strcat(message, message2);
            echoStringLen = strlen(message);
            if(send(clntSock, message, echoStringLen, 0) != echoStringLen)
                DieWithError("send() sent a different number of bytes than expected!");
            if(strcmp(message, "Bye\n") == 0)
                connectChat = false;
        }
        close(chatSock);
        // exit(0);
    }

    if(strcmp(clientOption, "5") == 0){
        memset(clientOption, 1, 0);
        if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
            DieWithError("recv() failed or connection closed prematurely");
        echoBuffer[bytesRcvd] = '\0'; /* Terminate the string! */
        if(strcmp(echoBuffer, "Client disconnected!\n") == 0) {    
            close(sock);
            printf("Server Socket Connection Closed\n");
        }

        printf("-------------disconnect with the server-----------\n");
        printf("Please enter your friend's IP address: ");
        scanf("%s", c);
        servlP = c;
        printf("Please enter your friend's port number: ");
        scanf("%hu", &echoServPort);
        printf("Connecting to your friend...\n");
        memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
        // echoServAddr.sin_family = AF_INET; /* Internet address family */
        // echoServAddr.sin_addr.s_addr = inet_addr(servlP); /* Server IP address */
        // echoServAddr.sin_port = htons(echoServPort); /* Server port */
        if((chatSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
            DieWithError(" socket failed");

        echoServAddr.sin_family = AF_INET; /* Internet address family */
        echoServAddr.sin_addr.s_addr = inet_addr(servlP); /* Server IP address */       
        echoServAddr.sin_port = htons(echoServPort); /* Server port */

        if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0) 
            DieWithError(" connect () failed");

        printf("Connected\n");
        echoStringLen = strlen(clientUsername);
        if(send(chatSock, clientUsername, echoStringLen, 0) != echoStringLen)
            DieWithError("send(): chat username(option5) sent a different number of bytes than expected!");
        if((recvMsgSize = recv(chatSock, clientUsername2, 30, 0)) < 0)
            DieWithError("recv() failed");
        connectChat = true;
        printf("Chat Enabled!\n");
        while(connectChat) {
            printf("(Type 'Bye' to stop the conversation!)\n");
            printf("%s: ", clientUsername);
            scanf("%s", message);
            fgets(message2, 1000, stdin);
            strcat(message, message2);
            echoStringLen = strlen(message);
            if(send(chatSock, message, echoStringLen, 0) != echoStringLen)
                DieWithError("send() sent a different number of bytes than expected");
            if(strcmp(message, "Bye\n") == 0)
                connectChat = false;
            if((recvMsgSize = recv(chatSock, message, 1000, 0)) < 0)
                DieWithError("recv() failed");
            message[recvMsgSize] = '\0';
            printf("%s: %s", clientUsername2, message);
            if(strcmp(message, "Bye\n") == 0)
                connectChat = false;
            memset(message, 0, 1000);
        }
        close(chatSock);
        // exit(0);
    }

    }

    printf("\n");  /* Print a final linefeed */

    close(sock);
    exit(0);
}
