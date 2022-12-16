#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h> 
#include <time.h>

#define SERVER_PORT 9999 // The port that the receiver listens
#define id1 7084
#define id2 383

int countmessarrive = 0;
double totaltime=0;
long sizefile;
void sendauthentication(int);

int main(int argc, char const *argv[])
{
	int listensocket = -1; // create listening socket
	if ((listensocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("Could not create listening socket : %d", errno);
		exit(1);
	}
	printf("Sokcet created\n");

	int yes = 1; // check if the ip in not in use
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
	if (bind(listensocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
	{
		printf("Bind failed with error code : %d", errno);
		close(listensocket);
		exit(1);
	}

	printf("Bind success\n");

	// Make the socket listening; actually mother of all client sockets.
	if (listen(listensocket, 1) == -1) // 1 is a Maximum size of queue connection requests
	{
		printf("listen failed with error code : %d", errno);
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
		printf("listen failed with error code : %d", errno);
		return -1;
	}

	printf("A new client connection accepted\n\n");
	char size[10] = {0};
	int recvmess = recv(clientSocket, size, sizeof(size), 0);
	if (recvmess == -1)
	{
		printf("Error in receiving size of file\n");
		exit(1);
	}
	else if (recvmess == 0)
	{
		printf("peer has closed the TCP connection prior to receive.\n");
		exit(1);
	}
	char *p;
	sizefile = strtol(size, &p, 10);
	int i = 0;
	while (1)
	{
		i++;
		char ccalgo[7]="reno";
        if (setsockopt(clientSocket, IPPROTO_TCP,TCP_CONGESTION, ccalgo, strlen(ccalgo)) != 0)
        {
            printf("Error in change cc reno\n");
            exit(1);
        }else{
			printf("change cc algo to reno \n");
		}
		long byteleft = sizefile / 2;
		char buffer[sizefile / 2];
		int bytesReceived = 0;
		clock_t start = clock();
		while (bytesReceived < sizefile / 2)
		{
			int messrecv = recv(clientSocket, buffer, byteleft, 0);
			bytesReceived += messrecv;
			byteleft -= messrecv;
			if (messrecv <= 0)
			{
				break;
			}
		}
		clock_t end = clock();
		double time= (double)(end - start)/CLOCKS_PER_SEC;
		countmessarrive++;
		totaltime += time;
		if(strcmp(buffer,"exit")==0){
			break;
		}
		printf("Received byte: %d in %f seconds \n\n", bytesReceived,time);
		sendauthentication(clientSocket);

		char ccalgo2[7]="cubic";
        if (setsockopt(clientSocket, IPPROTO_TCP,TCP_CONGESTION, ccalgo2, strlen(ccalgo2)) != 0)
        {
            printf("Error in change cc algo\n");
            exit(1);
        }else{
			printf("change cc algo to cubic \n");
		}

		byteleft = sizefile / 2;
		buffer[sizefile / 2];
		bytesReceived = 0;
		start = clock();
		while (bytesReceived < sizefile / 2)
		{
			int messrecv = recv(clientSocket, buffer, byteleft, 0);
			bytesReceived += messrecv;
			byteleft -= messrecv;
			if (messrecv <= 0)
			{
				break;
			}
		}
		end = clock();
		time= (double)(end - start)/CLOCKS_PER_SEC;
		countmessarrive++;
		totaltime += time;
		printf("Received byte: %d in %f seconds \n\n", bytesReceived,time);		
		sendauthentication(clientSocket);
	}
	printf("exit message arrived\n\n");
	close(listensocket); // close socket with sender
	printf("socket close\n\n");

	printf("total avarage time =  %f\n", totaltime/countmessarrive);

	return 0;
}

void sendauthentication(int sock)
{
	// send the authentication to the client
	char authentication[5];
	sprintf(authentication, "%d", id1 ^ id2);
	long bytesSent = 0;
	long bytesLeft = 5;
	while (bytesSent < 5)
	{
		long bytes = send(sock, authentication, bytesLeft, 0);
		if (bytes == -1)
		{
			printf("Error in sending authentication");
			exit(1);
		}
		else if (bytes == 0)
		{
			printf("peer has closed the TCP connection prior to send.\n");
			exit(1);
		}
		bytesSent += bytes;
		bytesLeft -= bytes;
	}
}
