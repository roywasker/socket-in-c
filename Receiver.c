#include <stdio.h>
#include <errno.h> 
#include <sys/types.h> 
#include <sys/socket.h>
#include <string.h> 
#include <arpa/inet.h>
#include <stdlib.h> 
#include <netinet/in.h>
#include <unistd.h>

#define SERVER_PORT 9999
#define SIZE 1024

void write_file(int sock);

int main(){

    int listensocket = -1;
    char buffer[SIZE];

    if((listensocket = socket(AF_INET , SOCK_STREAM , 0 )) == -1)
    {
        printf("Could not create listening socket : %d" ,errno);
        exit(1);
    }
    printf("Sokcet created\n");
    int yes=1; 
    if (setsockopt(listensocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
    { 
	    perror("setsockopt"); 
	    exit(1);
    }
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(SERVER_PORT);  

    if (bind(listensocket, (struct sockaddr *)&serverAddress , sizeof(serverAddress)) == -1)
    {
        printf("Bind failed with error code : %d" ,errno);
        close(listensocket);
        exit(1);
    }
      
    printf("Bind success\n");
  
    if (listen(listensocket, 1) == -1) 
	{	
	printf("listen failed with error code : %d",errno);
	    close(listensocket);
        exit(1);
    }

    printf("Waiting for incoming TCP-connections\n");
      
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLen = sizeof(clientAddress);

    memset(&clientAddress, 0, sizeof(clientAddress));
    int clientSocket = accept(listensocket, (struct sockaddr *)&clientAddress, &clientAddressLen);

    if (clientSocket == -1)
    {
        printf("listen failed with error code : %d",errno);
        return -1;
    }
      
    printf("A new client connection accepted\n");
    
    write_file(clientSocket);
    printf("successfully write to file\n");

    close(listensocket);
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