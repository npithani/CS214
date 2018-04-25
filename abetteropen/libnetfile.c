//
//  libnetfile.c
//  libnetfiles.c
//
//  Created by Priya Pithani on 4/25/18.
//  Copyright © 2018 Priya Pithani. All rights reserved.
//

#include "libnetfile.h"

#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>

int socketDescriptor;
int socket1;

int netserverinit(const char * hostname){
    
    //variables
    int socketfileDescriptor;
    int portNum;
    
    //structs
    //sockaddr_in is used to specify an endpoint address to which to connect a socket
    struct sockaddr_in connector;
    //hostent is used to represent an entry in the hosts database
    struct hostent *server;
    
    //random port number between (2^13) and (2^16)
    portNum = 9000;
    
    //create a socket and return a file descriptor that is a non-negative integer
    //AF_INET is an address family that is used to designate the type of addresses that your socket can communicate with (in this case, Internet Protocol v4 addresses). When you create a socket, you have to specify its address family, and then you can only use addresses of that type with the socket.
    //SOCK_STREAM is for two-way connections
    //protocol = 0 so that a default protocol is used
    socketfileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    
    //error-checking
    if (socketfileDescriptor < 0)
        return -1;
    
    //get structure of typr hostent for the given hostname
    server = gethostbyname(hostname);
    
    //error-checking
    if (server == NULL) {
        fprintf(stderr,"No such host, ERROR\n");
        return -1;
    }
    
    //erase data in connector by writing zeroes to that area
    memset(&connector, 0, sizeof(connector));
    
    connector.sin_family = AF_INET;
    
    //copy bytes from server to connector
    bcopy((char *)server->h_addr, (char *)&connector.sin_addr.s_addr, server->h_length);
    
    //The htons() function converts the unsigned short integer hostshort from host byte order to network byte order
    connector.sin_port = htons(portNum);
    
    //error-chcecking
    if (connect(socketfileDescriptor,(struct sockaddr *) &connector,sizeof(connector)) < 0)
        return -1;
    
    int socketDescriptor = socketfileDescriptor;
    return 0;
    
}//netserverinit(const char * hostname)

int netopen(const char *path, int flag){
    
    //check flags and assign value to state variable accordingly
    if(flag == O_RDONLY){
        state = O_RDONLY;
    }
    else if(flag == O_WRONLY){
        state = O_WRONLY;
    }
    else if(flag == O_RDWR){
        state = O_RDWR;
    }
    
    //create character array
    char line[255];
    
    sprintf(line, "%d", OPEN);
    
    //send code to server
    ssize_t toServer = write(socket1, line, strlen(line));
    
    //error-checking
    if(toServer != strlen(line)){
        fprintf(stderr, "The code can't be sent to the server: %s", strerror(errno));
        return -1;
    }

    sprintf(line, "%s", path);
    
    //send the name of the path to the server
    ssize_t sendPath = write(socket1, line, strlen(line));
    
    //error-checking
    if(sendPath != strlen(line)) {
        fprintf(stderr, "Path name could not be sent to the server: %s", strerror(errno));
        return -1;
    }
    
    sprintf(line, "%d", flag);
    //sends the server the flag
    ssize_t sent = write(socket1, line, strlen(line));
    
    //error-checking
    if (sent != strlen(line)) {
        fprintf(stderr, "Flags could not be sent to the server: %s", strerror(errno));
        return -1;
    }
 
    //empty the character array
    int k;
    for (k = 0; k < 255; k++) {
        line[k] = '\0';
    }
    
    //read a flag from the server
    int readCode = read(socket1, line, 1);
    int i = atoi(line); //atoi() returns zero if it encounter a string with no numerical sequence
    //if atoi() returned zero, the file can't be opened
    if(!i){
        fprintf(stderr, "File cannot be opened: %s", strerror(errno));
        return -1;
    }
    
    //empty the character array (again)
    int j;
    for (j = 0; j < 255; j++) {
        line[j] = '\0';
    }
    
    //get the file descriptor from the server
    int fileD = read(socket1, line, 255);
    return fileD;
    
} //netopen(const char *path, int flag)

ssize_t netread(int filedes, void* buff, size_t nbyte){
    
    //error-checking
    if(state == O_WRONLY){
        fprintf(stderr, "Request does not match: %s", strerror(errno));
        return -1;
    }

    //character array
    char line[255];
    
    sprintf(line, "%d", READ);
    
    //send the code to the server
    ssize_t sender = write(socketDescriptor, line, strlen(line));
    
    //error-checking
    if(sender != strlen(line)){
        fprintf(stderr, "The code cannot be sent to the server: %s", strerror(errno));
        return -1;
    }
    
    sprintf(line, "%d", filedes);
    //send the file escriptor to the server
    ssize_t sendFD = write(socketDescriptor, line, strlen(line));
    
    //error-checking
    if(sendFD != strlen(line)) {
        fprintf(stderr, "The file descriptor can't be sent to the server: %s", strerror(errno));
        return -1;
    }
    
    //empty the character array
    int k;
    for (k = 0; k < 255; k++) {
        line[k] = '\0';
    }

    sprintf(line, "%zu", nbyte);
    
    //send the number of bytes to the server
    ssize_t byteNum = write(socketDescriptor, line, strlen(line));
    
    //error-checking
    if (byteNum != strlen(line)) {
        fprintf(stderr, "Cannot send flags to server: %s", strerror(errno));
        return -1;
    }
    
    //empty the character array (again)
    int j;
    for (j = 0; j < 255; j++) {
        line[j] = '\0';
    }

    //read flag from server to determine success
    int reader = read(socketDescriptor, line, 1);
    int i = atoi(line); //atoi() returns zero if it encounter a string with no numerical sequence
    
    //error-checking
    if(!i){
        fprintf(stderr, "Cannot read file: %s", strerror(errno));
        return -1;
    }
    
    //empty the character array (again)
    int n;
    for (n = 0; n < 255; n++) {
        line[n] = '\0';
    }
 
    //read the number of bytes from the server
    int readByteNum = read(socketDescriptor, line, 255);
    int theBytes;
    sscanf(message, " %d", &theBytes);
    
    //empty the character array (again)
    int l;
    for (l = 0; l < 255; l++) {
        line[l] = '\0';
    }
    
    //reads as void* buff from client
    int reading = read(socketDescriptor, buff, 255);
    
    
    return theBytes;
    
    
}



