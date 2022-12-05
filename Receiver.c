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

int main(){

    int listensocket = -1;

    if((listensocket = socket(AF_INET , SOCK_STREAM , 0 )) == -1)
    {
        printf("Could not create listening socket : %d" ,errno);
    }

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
        return -1;
    }
      
    printf("Bind success\n");
  
    if (listen(listensocket, 1) == -1) 
	{	
	printf("listen failed with error code : %d",errno);
	    close(listensocket);
        return -1;
    }
    
    return 0;
}