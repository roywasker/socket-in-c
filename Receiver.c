#include <stdio.h>
#include <errno.h> 
#include <sys/types.h> 
#include <sys/socket.h>
#include <string.h> 
#include <arpa/inet.h>
#include <stdlib.h> 
#include <netinet/in.h>
#include <unistd.h>

#define SERVER_PORT 9999 //The port that the receiver listens
#define SIZE 1024 //size of buffer
#define id1 7084 
#define id1 0383

void write_file(int sock);

int main(){

    int listensocket = -1; // create listening socket
    if((listensocket = socket(AF_INET , SOCK_STREAM , 0 )) == -1)
    {
        printf("Could not create listening socket : %d" ,errno);
        exit(1);
    }
    printf("Sokcet created\n");

    int yes=1;  // check if the ip in not in use
    if (setsockopt(listensocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
    { 
	    perror("setsockopt"); 
	    exit(1);
    }

    //"sockaddr_in" used for IPv4 communication 
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(SERVER_PORT);  

    // Bind the socket to the port with any IP at this port
    if (bind(listensocket, (struct sockaddr *)&serverAddress , sizeof(serverAddress)) == -1)
    {
        printf("Bind failed with error code : %d" ,errno);
        close(listensocket);
        exit(1);
    }
      
    printf("Bind success\n");
    
    // Make the socket listening; actually mother of all client sockets.
    if (listen(listensocket, 1) == -1) //1 is a Maximum size of queue connection requests
	{	
	printf("listen failed with error code : %d",errno);
	    close(listensocket);
        exit(1);
    }

    printf("Waiting for incoming TCP-connections\n");


    struct sockaddr_in clientAddress;
    socklen_t clientAddressLen = sizeof(clientAddress);
    memset(&clientAddress, 0, sizeof(clientAddress));

    int clientSocket = accept(listensocket, (struct sockaddr *)&clientAddress, &clientAddressLen); // create socket to sender
    if (clientSocket == -1)
    {
        printf("listen failed with error code : %d",errno);
        return -1;
    }
      
    printf("A new client connection accepted\n");
    
    write_file(clientSocket); // receive the file 
    printf("successfully write to file\n");

    close(listensocket); // close socket with sender
    printf("socket close\n");

    return 0;
}

void write_file(int sock){
    int recvmess;
    FILE *fp;
    char *filename ="recfile.txt";
    char buffer[SIZE];

    fp=fopen(filename, "w");

    if (fp == NULL)
    {
        perror("Can't get filename");
        exit(1);
    }

    while (1)
    {
        recvmess=recv(sock,buffer ,SIZE ,0);
        if (recvmess <= 0)
        {
           break;
           return;
        }
        fprintf(fp, "%s" ,buffer);
        bzero(buffer ,SIZE);
    }
    fclose(fp);
    return;
}