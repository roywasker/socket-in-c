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
#define SERVER_IP_ADDRESS "0.0.0.0" 
#define SIZE 1024 // size of buffer
#define SENDFILE "text.txt" // the file to send
#define id1 7084
#define id1 0383

void send_file(FILE *fp , int sock);

int main(){
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock==-1)
    {
        printf("Unable to create a socket : %d",errno);
    }

    //"sockaddr_in" used for IPv4 communication 
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
	int rval = inet_pton(AF_INET, (const char*)SERVER_IP_ADDRESS, &serverAddress.sin_addr);
	if (rval <= 0)
	{
		printf("inet_pton failed");
		exit(1);
	}
    
    // Make a connection to the receiver with socket
    if (connect(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1)
    {
	    printf("connect failed with error code : %d",errno);
    }

    printf("connected to server\n");

    FILE *fp;
    char *filename = SENDFILE; 

    fp=fopen(filename, "r"); // open file to send

    if (fp == NULL) // check if the file open successfully
    {
        perror("Can't get filename");
        exit(1);
    }

    send_file(fp,sock);
    printf("file send successfully\n"); // sending the file to recevier

    /*char sendagain[3];
    char exitloop[3];

    do
    {
        printf("Send the file again?\n");
        scanf("%s",sendagain);
      while (strcmp(sendagain ,"yes")==0)
    {
        send_file(fp,sock);
        printf("file send successfully\n"); // sending the file to recevier
        printf("Send the file again?\n");
        scanf("%s",sendagain);
    }
    printf("exit ?");
    scanf("%s",exitloop);
    } while (strcmp(exitloop ,"yes")==0);*/
    
    
    close(sock); // close socket
    printf("socket close\n");

    fclose(fp); // close file
    return 0;
}

void send_file(FILE *fp , int sock){
    char data[SIZE]={0};
    char buffer[SIZE]={0};
    while (fgets(data, SIZE ,fp) != NULL)
    {
        int bytesSent = send(sock, data, sizeof(data), 0);
        int revcmess=recv(sock, buffer,sizeof(buffer) ,0);
        if (bytesSent== -1)
        {
            printf("Error in sending file");
            exit(1);
        }else if (bytesSent == 0)
        {
           printf("peer has closed the TCP connection prior to send.\n");
        }else if (sizeof(data) > bytesSent)
        {
	        printf("sent only %ld bytes from the required %d.\n", sizeof(data), bytesSent);
        }
        bzero(data ,SIZE);
    }
}