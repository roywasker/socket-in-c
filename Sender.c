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
#define id2 383 // 4 last dig is 0383

long get_file_len(FILE *fp);
int checkauthentication(int sock);

int main(int argc, char const *argv[])
{
	FILE *fp;
	char *filename = SENDFILE;

	fp = fopen(filename, "r"); // open file to send

	if (fp == NULL) // check if the file open successfully
	{
		perror("Can't get filename");
		exit(1);
	}
	long sizefile = get_file_len(fp);

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		printf("Unable to create a socket : %d", errno);
	}

	//"sockaddr_in" used for IPv4 communication
	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(PORT);
	int rval = inet_pton(AF_INET, (const char *)SERVER_IP_ADDRESS, &serverAddress.sin_addr);
	if (rval <= 0)
	{
		printf("inet_pton failed");
		exit(1);
	}

	// Make a connection to the receiver with socket
	if (connect(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
	{
		printf("connect failed with error code : %d", errno);
	}

	printf("connected to server\n\n");

	char mesofsizefile[sizeof(sizefile)];
	sprintf(mesofsizefile, "%ld", sizefile);
	long bytesSent = send(sock, mesofsizefile, sizeof(mesofsizefile), 0);
	if (bytesSent == -1)
	{
		printf("Error in sending size of file");
		exit(1);
	}
	else if (bytesSent == 0)
	{
		printf("peer has closed the TCP connection prior to send.\n");
	}
	while (1)
	{
		char ccalgo[7] = "reno";
		if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, ccalgo, strlen(ccalgo)) != 0)
		{
			printf("Error in change cc reno\n");
			exit(1);
		}
		else
		{
			printf("change cc algo to reno \n\n");
		}
		char buffer[sizefile / 2];
		fp = fopen(filename, "r");
		long b = 0;
		long bytesSent = 0;
		long sizefiletosend = sizefile / 2;
		long bytesLeft = sizefiletosend;
		while ((b = fread(buffer, 1, bytesLeft, fp)) > 0)
		{
			int sendmess = send(sock, buffer, b, 0); // int bytesSent = send(sock, buffer, sizeof(buffer), 0);
			if (-1 == sendmess)
			{
				printf("Error in sending file: %d", errno);
			}
			else if (0 == sendmess)
			{
				printf("peer has closed the TCP connection prior to send().\n");
			}
			else if (b > sendmess) //(sizeof(buffer) > bytesSent)
			{
				printf("sent only %d bytes from the required %ld.\n", sendmess, b); // printf("sent only %d bytes from the required %d.\n", bytesSent, sizeof(buffer));
			}
			else
			{
				// printf("Message was successfully sent. Send %d bytes: \n", bytesSent);
			}
			bytesSent += sendmess;
			bytesLeft -= sendmess;
		}
		printf("Send %ld bytes of file.\n", bytesSent);
		printf("first part of file send successfully\n");

		int cheaut = checkauthentication(sock);

		if (cheaut == 1)
		{
			printf("authentication for first part are successfully\n\n\n");
		}
		else
		{
		}
		char ccalgo2[7] = "cubic";
		if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, ccalgo2, strlen(ccalgo2)) != 0)
		{
			printf("Error in change cc algo\n");
			exit(1);
		}
		else
		{
			printf("change cc algo to cubic \n\n");
		}
		b = 0;
		bytesSent = 0;
		sizefiletosend = sizefile / 2;
		bytesLeft = sizefiletosend;
		bzero(buffer, sizeof(buffer));
		while ((b = fread(buffer, 1, bytesLeft, fp)) > 0)
		{ // while(fgets(buffer, sizeof(buffer), fp1) != NULL) {
			//	send(sock, buffer, b, 0);
			int sendmess = send(sock, buffer, b, 0); // int bytesSent = send(sock, buffer, sizeof(buffer), 0);
			if (-1 == sendmess)
			{
				printf("Error in sending file: %d", errno);
			}
			else if (0 == sendmess)
			{
				printf("peer has closed the TCP connection prior to send().\n");
			}
			else if (b > sendmess) //(sizeof(buffer) > bytesSent)
			{
				printf("sent only %d bytes from the required %ld.\n", sendmess, b); // printf("sent only %d bytes from the required %d.\n", bytesSent, sizeof(buffer));
			}
			else
			{
				// printf("Message was successfully sent. Send %d bytes: \n", bytesSent);
			}
			bytesSent += sendmess;
			bytesLeft -= sendmess;
		}
		printf("Send %ld bytes of file.\n", bytesSent);
		printf("second part of file send successfully\n");

		cheaut = checkauthentication(sock);

		if (cheaut == 1)
		{
			printf("authentication for second part are successfully\n\n\n");
		}
		else
		{
		}
		fclose(fp);
		char ch1, ch2;
		printf("Send the file again? y to yes or n to no\n");
		ch2 = getchar();
		if (ch2 == 'n')
		{
			char exitmess[]="exit";
            int bytesSent = send(sock, exitmess, sizeof(exitmess) ,0);
            if (bytesSent== -1)
            {
                printf("Error in sending size of file");
                exit(1);
            }else if (bytesSent == 0)
            {
                printf("peer has closed the TCP connection prior to send.\n");
            }
            break;
		}else if (ch2 =='y')
		{
			continue;
		}
		
	}
	printf("close soket\n");
	close(sock);
	return 0;
}
long get_file_len(FILE *fp)
{
	long size = 0;
	if (fp == NULL)
	{ // check if the file open successfully
		return 0;
	}
	fseek(fp, 0, SEEK_END); // move file pointer to end of file
	size = ftell(fp);		// calculate the size of the file
	return size;
}

int checkauthentication(int sock)
{
	// receive the authentication code from the server
	char auth[5];
	char temp[5];
	sprintf(temp, "%d", id1 ^ id2);
	int bytes = recv(sock, auth, sizeof(auth), 0);
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
	if (strcmp(auth, temp) == 0)
	{
		return 1;
	}
	return 0;
}