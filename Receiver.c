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

int CountMessArrive = 0; // count how much time sender send part of the file 
double TotalTime=0; // sum all the time
long SizeFile=1048580; 

void sendauthentication(int);

int main(int argc, char const *argv[])
{
	int listenSocket = -1; // create listening socket
	if ((listenSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("Could not create listening socket : %d", errno);
		exit(1);
	}
	printf("Sokcet created\n");

	int yes = 1; // check if the ip in not in use
	if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
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
	if (bind(listenSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
	{
		printf("Bind failed with error code : %d", errno);
		close(listenSocket);
		exit(1);
	}

	printf("Bind success\n");

	// Make the socket listening; actually mother of all client sockets.
	if (listen(listenSocket, 1) == -1) // 1 is a Maximum size of queue connection requests
	{
		printf("listen failed with error code : %d", errno);
		close(listenSocket);
		exit(1);
	}

	printf("Waiting for incoming TCP-connections\n");

	struct sockaddr_in ClientAddress;
	socklen_t clientAddressLen = sizeof(ClientAddress);
	memset(&ClientAddress, 0, sizeof(ClientAddress)); // file struct with 0

	int ClientSocket = accept(listenSocket, (struct sockaddr *)&ClientAddress, &clientAddressLen); // create socket to sender
	if (ClientSocket == -1)
	{
		printf("listen failed with error code : %d", errno);
		return -1;
	}

	printf("A new client connection accepted\n\n");
	double times[100]; // assuming that send the file not over the 50 times
	while (1)
	{
		char ccalgo[7]="reno";
        if (setsockopt(ClientSocket, IPPROTO_TCP,TCP_CONGESTION, ccalgo, strlen(ccalgo)) != 0) //change cc algorithm to reno
        {
            printf("Error in change cc reno\n");
            exit(1);
        }else{
			printf("change cc algo to reno \n");
		}
		long BytesLeft = SizeFile/2; // intialize how much byte left to received
		char buffer[SizeFile /2];
		long BytesReceived = 0; // countig how much byte received from sender
		clock_t start = clock(); // start measure time
		while (BytesReceived < SizeFile/2)
		{
			int MessRecv = recv(ClientSocket, buffer, BytesLeft, 0); // receive the message 
			BytesReceived += MessRecv; // add the number of byte that arrive from sender
			BytesLeft -= MessRecv; // subtraction the number of byte that left to receive
			if (MessRecv <= 0)
			{
				break;
			}
		}
		clock_t end = clock(); // stop measure time
		double time= (double)(end - start)/CLOCKS_PER_SEC; // calculating the time to take the file to arrive 
		times[CountMessArrive++]=time;
		TotalTime += time; // add time to total time 
		if(strcmp(buffer,"exit")==0){ // check if get exit message
			break;
		}
		printf("Received  %ld bytes\n\n", BytesReceived); // print how much byte arrive the how much time its take  
		sendauthentication(ClientSocket); // send authentication

		char ccalgo2[7]="cubic";
        if (setsockopt(ClientSocket, IPPROTO_TCP,TCP_CONGESTION, ccalgo2, strlen(ccalgo2)) != 0) //change cc algorithm to reno
        {
            printf("Error in change cc algo\n");
            exit(1);
        }else{
			printf("change cc algo to cubic \n");
		}

		BytesLeft = SizeFile / 2; // intialize how much byte left to received
		buffer[SizeFile / 2];
		BytesReceived = 0;// countig how much byte received from sender
		start = clock(); // start measure time
		while (BytesReceived < SizeFile/2)
		{
			int MessRecv = recv(ClientSocket, buffer, BytesLeft, 0); // receive the massage 
			BytesReceived += MessRecv; // add the number of byte that arrive from sender
			BytesLeft -= MessRecv;  // subtraction the number of byte that left to receive
			if (MessRecv <= 0)
			{
				break;
			}
		}
		end = clock();// stop measure time
		time= (double)(end - start)/CLOCKS_PER_SEC; // calculating the time to take the file to arrive 
		times[CountMessArrive++]=time;
		TotalTime += time; // add time to total time 
		printf("Received %ld byte\n\n", BytesReceived);	 // print how much byte arrive the how much time its take  
		sendauthentication(ClientSocket); // send authentication
		
	}
	CountMessArrive--; // because exit message
	printf("\nexit message arrived\n\n");
	close(listenSocket); // close socket with sender
	printf("socket close\n\n");
	double TimeForFirstPart=0,TimeForSecondPart=0;
	for (int i = 0; i <CountMessArrive; i=i+2)
	{
		TimeForFirstPart+=times[i];
		TimeForSecondPart+=times[i+1];
		printf("time to receive first part in %d time is : %f\n",(i/2)+1,times[i]); //print how much time its take to receive the part of file each send
		printf("time to receive second part in %d time is : %f\n",(i/2)+1,times[i+1]); //print how much time its take to receive the part of file each send
	}
	printf("total avarage time for firts part =  %f\n", TimeForFirstPart/((CountMessArrive)/2)); // print avarage time
	printf("total avarage time for second part =  %f\n", TimeForSecondPart/((CountMessArrive)/2)); // print avarage time
	printf("total avarage time =  %f\n", TotalTime/CountMessArrive); // print avarage time

	return 0;
}

void sendauthentication(int sock) // send the authentication to the sender
{
	char authentication[5];
	sprintf(authentication, "%d", id1 ^ id2); // calculating the xor of id's
	long BytesSent = 0;
	long BytesLeft = 5;
	while (BytesSent < 5)
	{
		long bytes = send(sock, authentication, BytesLeft, 0); // send the authentication
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
		BytesSent += bytes;
		BytesLeft -= bytes;
	}
}
