# ChatServer CSC645 Computer Networks

C program to emulate Server-Client connection and P2P connection using websockets and TCP for communication and connectivity. 

## Built With
* [C](https://www.learn-c.org/)

## Compilation

### Client

```
gcc *.c -o client
```

### Server

```
gcc *.c -o server
```

## Run

### Client

```
./client
```

### Server

```
./server
```

## Usage

### Client

Will prompt the user a menu:

0. Connect to Server
1. Get the user list
2. Send a message
3. Get my messages
4. Initialize a chat with my friend
5. Chat with my friend

Options (0-3) the client will send that request to the server and the server will return a response

Option 4 will create a socket connection that will enable chat between peers

Option 5 will prompt the client the server IP addr and port number of which user he/she is trying to start a chat with.

### Server

Is simply an endpoint for the client to connect to, request information, and store information 
