/******************************************************************************
* myClient.c
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

void sendToServer(int socketNum, uint8_t *buffer, int len);
int readFromStdin(uint8_t * buffer);
void checkArgs(int argc, char * argv[]);

void processMsgFromServer(int socketNum){
	uint8_t dataBuffer[MAXBUF];
	int messageLen = 0;
	
	//now get the data from the client_socket
	if ((messageLen = recvPDU(socketNum, dataBuffer, MAXBUF)) < 0)
	{
		perror("recv call");
		exit(-1);
	}

	if (messageLen > 0)
	{
		/*Printing what the server sent*/
		printf("%s\n", dataBuffer);
	}
	else
	{
		printf("Connection closed by other side\n");
		/*removing from poll set if the connection is closed*/
		close(socketNum);
		removeFromPollSet(socketNum);
	}
}

void clientControl(int socketNum){
	int pollCheck;
	uint8_t sendBuf[MAXBUF];
	int sendLen;
	setupPollSet();
	addToPollSet(socketNum);
	addToPollSet(STDIN_FILENO);
	while(1){
		/*begin the process of asking the user for their message*/
		sendLen = readFromStdin(sendBuf);
		
		pollCheck = pollCall(10000);
		if(pollCheck < 0){
			printf("pollCall() Timed Out\n");
		}
		else if(pollCheck == socketNum){
			/*Server sent a message*/
			processMsgFromServer(socketNum);
			
		}
		else if(pollCheck == STDIN_FILENO){
			printf("read: %s string len: %d (including null)\n", sendBuf, sendLen);
			/*User put in a message, time to send it*/
			sendToServer(socketNum, sendBuf, sendLen);
		}
	}
}

int main(int argc, char * argv[])
{
	int socketNum = 0;         //socket descriptor
	
	checkArgs(argc, argv);

	/* set up the TCP Client socket  */
	socketNum = tcpClientSetup(argv[1], argv[2], DEBUG_FLAG);
	
	clientControl(socketNum);
	
	close(socketNum);
	
	return 0;
}

void sendToServer(int socketNum, uint8_t *sendBuf, int sendLen)
{
	/*uint8_t sendBuf[MAXBUF];*/   //data buffer
	/*int sendLen = 0; */       //amount of data to send
	int sent = 0;            //actual amount of data sent/* get the data and send it   */
	
	/*sendLen = readFromStdin(sendBuf);*/
	/*printf("read: %s string len: %d (including null)\n", sendBuf, sendLen);*/
	
	sent =  sendPDU(socketNum, sendBuf, sendLen);
	if (sent < 0)
	{
		perror("send call");
		printf("Server has terminated or sendPDU failed\n");
		exit(-1);
	}
	else if(sent == 0){
		printf("Server has terminated\n");
		close(socketNum);
		exit(0);
	}
	else{
		printf("Amount of data sent is: %d\n", sent);
	}
	
}

int readFromStdin(uint8_t * buffer)
{
	char aChar = 0;
	int inputLen = 0;        
	
	// Important you don't input more characters than you have space 
	buffer[0] = '\0';
	printf("Enter data: ");
	while (inputLen < (MAXBUF - 1) && aChar != '\n')
	{
		aChar = getchar();
		if (aChar != '\n')
		{
			buffer[inputLen] = aChar;
			inputLen++;
		}
	}
	
	// Null terminate the string
	buffer[inputLen] = '\0';
	inputLen++;
	
	return inputLen;
}

void checkArgs(int argc, char * argv[])
{
	/* check command line arguments  */
	if (argc != 3)
	{
		printf("usage: %s host-name port-number \n", argv[0]);
		exit(1);
	}
}

/*void clientControl(int socketNum){*/
	/*continuosly loop and call stdin function for more things to send to the server*/
