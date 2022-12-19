#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#define PORT 9999
#define SERVER_IP_ADDRESS "0.0.0.0"
#define SENDFILE "text.txt" // the file to send
#define id1 7084
#define id2 383 // first dig is 0 so 0383

int checkauthentication(int sock);
long SizeFile=1048580 ; 

int main()
{
    FILE *fp;
    char *filename = SENDFILE;

    fp = fopen(filename, "r"); // open file to send

    if (fp == NULL) // check if the file open successfully
    {
        perror("Can't get filename");
        exit(1);
    }
    
    char message[SizeFile];

    fread(message,sizeof(char),SizeFile,fp); // insert the file in to the string

    fclose(fp); // close the file
    int sock = socket(AF_INET, SOCK_STREAM, 0); // create socket
    if (sock == -1)
    {
        printf("Unable to create a socket : %d", errno);
        exit(1);
    }

    //"sockaddr_in" used for IPv4 communication
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress)); // file struct with 0
    serverAddress.sin_family = AF_INET; // work with ipv4
    serverAddress.sin_port = htons(PORT); // insert to struct the port 
    int rval = inet_pton(AF_INET, (const char *)SERVER_IP_ADDRESS, &serverAddress.sin_addr);//convert the address to binary
    if (rval <= 0)
    {
        printf("inet_pton failed");
        exit(1);
    }

    if (connect(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) // Make a connection to the receiver with socket
    {
        printf("connect failed with error code : %d", errno);
    }

    printf("connected to server\n\n");

    while (1)
    {
        char ccalgo[7] = "reno";
        if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, ccalgo, strlen(ccalgo)) != 0) //change cc algorithm to reno
        {
            printf("Error in change cc algorithm to reno\n");
            exit(1);
        }
        else
        {
            printf("change cc algorithm to reno \n\n");
        }
        long BytesSent = 0; // countig how much byte are send to recevier
        long SizeFileToSend = SizeFile / 2; // amont of half file
        long BytesLeft = SizeFileToSend; // intialize how much byte left to send
        while (BytesSent<SizeFileToSend)
        {
            int SendMess = send(sock, message+BytesSent,BytesLeft, 0); // send message to reciver 
            if (-1 == SendMess)
            {
                printf("Error in sending file: %d", errno);
            }
            else if (0 == SendMess)
            {
                printf("peer has closed the TCP connection prior to send().\n");
            }
            BytesSent += SendMess; // add the number of byte that arrive to recevier
            BytesLeft -= SendMess; // subtraction the number of byte that arrive recevier
        }
        printf("Send %ld bytes of file.\n", BytesSent); // print how much byte send to recevier
        printf("first part of file send successfully\n");

        int cheaut = checkauthentication(sock); // cheack authentication

        if (cheaut == 1) // retun 1 if the authentication is correct
        {
            printf("authentication for first part are successfully\n\n\n");
        }else{
            break;
        }
        char ccalgo2[7] = "cubic"; 
        if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, ccalgo2, strlen(ccalgo2)) != 0) //change cc algorithm to cubic
        {
            printf("Error in change cc algo\n");
            exit(1);
        }
        else
        {
            printf("change cc algo to cubic \n\n");
        }
        BytesSent = 0; // countig how much byte are send to recevier
        BytesLeft = SizeFileToSend; // intialize how much byte left to send
        while (BytesSent< SizeFileToSend)
        { 
            int SendMess = send(sock,message+BytesSent+SizeFile/2,BytesLeft, 0); //send message to reciver 
            if (-1 == SendMess)
            {
                printf("Error in sending file: %d", errno);
            }
            else if (0 == SendMess)
            {
                printf("peer has closed the TCP connection prior to send().\n");
            }
            BytesSent += SendMess; // add the number of byte that arrive recevier
            BytesLeft -= SendMess; // subtraction the number of byte that arrive recevier
        }
        printf("Send %ld bytes of file.\n", BytesSent); // print how much byte send to recevier
        printf("second part of file send successfully\n");

        cheaut = checkauthentication(sock); // cheack authentication

        if (cheaut == 1)
        {
            printf("authentication for second part are successfully\n\n\n");
        }else{
            break;
        }
        char ch1 , ch2 ;
        printf("Send the file again? y to yes or n to no\n"); //check if send file again
        ch2 = getchar();
        getchar();
        if (ch2 == 'n'||ch2 == 'N') // if no check if exit
        {
            printf("exit? y to yes or n to no\n");
            ch1 = getchar();
            getchar();
            if (ch1 == 'y'||ch1 == 'Y') //if exit 
            {
                char ExitMess[] = "exit";
                int BytesSent = send(sock, ExitMess, sizeof(ExitMess), 0); // send exit message to recevier
                if (BytesSent == -1)
                {
                    printf("Error in sending size of file");
                    exit(1);
                }
                else if (BytesSent == 0)
                {
                    printf("peer has closed the TCP connection prior to send.\n");
                }
                break; // break loop
            }
        }
        else if (ch2 == 'y')
        {
            continue;
        }
    }
    printf("\nclose soket\n"); 
    close(sock); // close soket 
    return 0;
}

int checkauthentication(int sock)
{
    // receive the authentication code from the server
    char auth[5];
    char XorIds[5];
    sprintf(XorIds, "%d", id1 ^ id2); // insert to temp id1 xor id2
    int bytes = recv(sock, auth, sizeof(auth), 0); // recving the authentication form reciver 
    if (bytes == -1)
    {
        printf("Error in receiving authentication code\n");
        exit(1);
    }
    else if (bytes == 0)
    {
        printf("peer has closed the TCP connection prior to receive.\n");
        exit(1);
    }
    if (strcmp(auth, XorIds) == 0) //cheack if the authentication from reciver is equal to id1 xor id2
    {
        return 1;
    }
    return 0;
}