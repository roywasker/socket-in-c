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
#define SENDFILE "text.txt" // the file to send
#define id1 7084
#define id2 383 // 4 last dig is 0383

void send_file(char [], int sock);
long get_file_len(FILE *fp);

int main(){
    FILE *fp;
    char *filename = SENDFILE; 

    fp=fopen(filename, "r"); // open file to send

    if (fp == NULL) // check if the file open successfully
    {
        perror("Can't get filename");
        exit(1);
    }
    long sizefile=get_file_len(fp);

    fp=fopen(filename, "r");

    char message[sizefile+1];

    fread(message,sizeof(char),sizefile,fp);

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

    char mesofsizefile[sizeof(sizefile)];
    sprintf(mesofsizefile, "%ld", sizefile);
    long bytesSent = send(sock, mesofsizefile, sizeof(mesofsizefile) ,0);
    if (bytesSent== -1)
    {
        printf("Error in sending size of file");
        exit(1);
    }else if (bytesSent == 0)
    {
        printf("peer has closed the TCP connection prior to send.\n");
    }

    send_file(message,sock); // send the first part 

    printf("first part of file send successfully\n");

    char auth[4];
    char temp [4];
    sprintf(temp, "%ld", id1^id2);
    recv(sock,auth,sizeof(auth),0);
    if (strcmp(auth,temp)==0)
    {
        printf("authentication successfully\n");
    }else{
        printf("authentication not successfully\n");
    }
    send_file(message+(sizefile/2),sock); // send the second part 

    printf("second part of file send successfully\n");
    
    close(sock); // close socket
    printf("socket close\n");

    fclose(fp); // close file
    return 0;
}

void send_file(char *mes, int sock){
    int finish = strlen(mes)/2;
    int countbit=0;
    
    long bytesSent = send(sock, mes, finish, 0);
    if (bytesSent== -1)
    {
        printf("Error in sending file");
        exit(1);
    }else if (bytesSent == 0)
    {
        printf("peer has closed the TCP connection prior to send.\n");
    }else if (finish > bytesSent)
    {
	    printf("sent only %ld bytes from the required %d.\n",bytesSent,finish);
    }
}
long get_file_len(FILE *fp){
    long size=0;
    if (fp==NULL){ // check if the file open successfully
        return 0;
    }
    fseek (fp,0,SEEK_END);  //move file pointer to end of file
    size= ftell(fp); //calculate the size of the file
    return size;
}