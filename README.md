# Tcp Chat application

This is a tcp chat app, it uses the TCP/IP protocol for communication, and allows for multi client connection.

## Features
When ran, a server is created that listens for incoming connection from multiple clients, and when this connection is established, we can send data across from multple clients to server and each other clients receive the same data.

- Multiple simultaneous clients using `poll()`
- Real-time message broadcasting
- Username support
- Join notifications

## How to compile
- Compile the server program
```bash
gcc -Wall -Wextra server.c helper.c -o server
```
- Compile the client program
```bash
gcc -Wall -Wextra client.c helper.c -o client
```

## How to run
- On one terminal run the server
```bash
./server
```
- On multiple terminals you can run the client with the user name
```bash
./client <username>
```