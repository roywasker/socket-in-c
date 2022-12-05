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
#define SIZE 1024
#define SENDFILE "text.txt"

void send_file(FILE *fp , int sock);

int main(){

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock==-1)
    {
        printf("Unable to create a socket : %d",errno);
    }

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

    if (connect(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1)
    {
	    printf("connect failed with error code : %d",errno);
    }

    printf("connected to server\n");

    FILE *fp=NULL;
    char *filename = SENDFILE; 

    fp=fopen(filename, "r");

    if (fp == NULL)
    {
        perror("Can't get filename");
        exit(1);
    }

    send_file(fp,sock);
    printf("file send successfully\n");

    close(sock);
    printf("socket close\n");

    fclose(fp);
    fp=NULL;
    return 0;
}

void send_file(FILE *fp , int sock){
    char data[SIZE]={0};

    while (fgets(data, SIZE ,fp) != NULL)
    {
        if (send(sock, data, sizeof(data), 0)== -1)
        {
            printf("Error in sending file");
            exit(1);
        }
        bzero(data ,SIZE);
    }
    
}