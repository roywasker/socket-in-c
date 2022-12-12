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
#define id1 7084 
#define id2 383

void rec_file(int sock);
void sendauthentication(int);
void calculateauthentication(char temp[]);

long sizefile;

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

    char size[10]={0};
    int recvmess = recv(clientSocket,size ,sizeof(size) ,0);
    char *p;
    sizefile=strtol(size,&p,10);

    rec_file(clientSocket); // receive the file 
    printf("successfully write to first file\n");

    close(listensocket); // close socket with sender
    printf("socket close\n");

    return 0;
}

void rec_file(int sock){
    int recvmess;
    char buffer [sizefile];
    recvmess=recv(sock,buffer ,sizefile ,0);
    if (recvmess <= 0)
    {
        return;
    }
    bzero(buffer ,sizefile);
    sendauthentication(sock);
}
void sendauthentication(int sock){
    char authentication[sizeof(sizefile)];
    calculateauthentication(authentication);
    printf("%s-------------------------\n",authentication);
    long bytesSent = send(sock, authentication, sizeof(authentication) ,0);
    if (bytesSent== -1)
    {
        printf("Error in sending authentication");
        exit(1);
    }else if (bytesSent == 0)
    {
        printf("peer has closed the TCP connection prior to send.\n");
    }
}
long dectobin(int dec) {
    long bin = 0;
    int rem, i = 1;
    while (dec!=0) {
        rem = dec % 2;
        dec /= 2;
        bin += rem * i;
        i *= 10;
  }
  return bin;
}
void calculateauthentication(char temp[]){
    long idf=dectobin(id1);
    long ids =dectobin(id2);
    char xorid[sizeof(char)*16];
    sprintf(xorid, "%ld", idf^ids);
    strcpy(temp,xorid);
}