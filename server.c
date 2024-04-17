/******************************************************************************
* myServer.c
* 
* Writen by Prof. Smith, updated Jan 2023
* Use at your own risk.  
*
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdint.h>

#include "networks.h"
#include "safeUtil.h"
#include "sendrcv.h"
#include "pollLib.h"

#define MAXBUF 1024
#define DEBUG_FLAG 1

void recvFromClient(int clientSocket);
int checkArgs(int argc, char *argv[]);
void processClient(int clientSk);
void addNewSocket(int mainSk);
void serverControl(int mainSk);

int main(int argc, char *argv[])
{
	int mainServerSocket = 0;   //socket descriptor for the server socket
	/*int clientSocket = 0;*/   //socket descriptor for the client socket
	int portNumber = 0;
	
	portNumber = checkArgs(argc, argv);
	
	//create the server socket
	mainServerSocket = tcpServerSetup(portNumber);

	serverControl(mainServerSocket);
	// wait for client to connect
	/*clientSocket = tcpAccept(mainServerSocket, DEBUG_FLAG);

	recvFromClient(clientSocket);
	
	// close the sockets 
	close(clientSocket);*/
	close(mainServerSocket);

	
	return 0;
}

void recvFromClient(int clientSocket)
{
	uint8_t dataBuffer[MAXBUF];
	int messageLen = 0;
	
	//now get the data from the client_socket
	if ((messageLen = recvPDU(clientSocket, dataBuffer, MAXBUF)) < 0)
	{
		perror("recv call");
		exit(-1);
	}

	if (messageLen > 0)
	{
		printf("Message received, length: %d Data: %s\n", messageLen, dataBuffer);
	}
	else
	{
		printf("Connection closed by other side\n");
	}
}

int checkArgs(int argc, char *argv[])
{
	// Checks args and returns port number
	int portNumber = 0;

	if (argc > 2)
	{
		fprintf(stderr, "Usage %s [optional port number]\n", argv[0]);
		exit(-1);
	}
	
	if (argc == 2)
	{
		portNumber = atoi(argv[1]);
	}
	
	return portNumber;
}

void addNewSocket(int mainSocket){
	/*processes new connection, accept() and adding to poll set*/
	int newSocket;
	newSocket = tcpAccept(mainSocket, DEBUG_FLAG);
	addToPollSet(newSocket);

}

void processClient(int clientSocket){
	/*calls recvPDU() and outputs the message*/
	/* Message received on socket 5, length: 6 Data: hello */
	uint8_t dataBuffer[MAXBUF];
	int messageLen = 0;
	
	//now get the data from the client_socket
	if ((messageLen = recvPDU(clientSocket, dataBuffer, MAXBUF)) < 0)
	{
		perror("recv call");
		exit(-1);
	}

	if (messageLen > 0)
	{
		printf("Message received on socket %d, length: %d Data: %s\n", clientSocket, messageLen, dataBuffer);
	}
	else
	{
		printf("Connection closed by other side\n");
		/*removing from poll set if the connection is closed*/
		close(clientSocket);
		removeFromPollSet(clientSocket);
	}
}


void serverControl(int mainSocket){
	/*Add main server socket to poll set*/

	/* while(1)*/
		/* call poll()*/
		/*if poll() returns main server socket, call addNewSocket()*/
		/*if poll() returns a client socket, call processClient()*/

	int pollCheck;
	setupPollSet();
	addToPollSet(mainSocket);

	while(1){
		pollCheck = pollCall(10000);
		if(pollCheck < 0){
			printf("Poll() Timed Out.\n");
		}
		else if(pollCheck == mainSocket){
			addNewSocket(mainSocket);
		}
		else{
			processClient(pollCheck);
		}
	}
	
}