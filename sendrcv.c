#include "sendrcv.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>


/*Was getting "unknown type name uint8_t" error so replaced with char"*/
int sendPDU(int clientSocket, char *dataBuffer, int lengthOfData){
    char *sendPDU = (char *)malloc(sizeof(char) * (lengthOfData + 2));
    uint16_t pduLen = htons(lengthOfData);
    int numSent;

    /*add the length to the begginning of the pdu*/
    memcpy(sendPDU, &pduLen, sizeof(uint16_t));

    /*Copying the databuffer*/
    memcpy(sendPDU, dataBuffer + (sizeof(uint16_t)), sizeof(char) * lengthOfData);

    /*Sending*/
    numSent = send(clientSocket, sendPDU, lengthOfData + 2, 0);
    if(numSent < 0){
        /*Error*/
        perror("send error ");
    }

    return numSent;
}
int recvPDU(int socketNumber, char *dataBuffer, int bufferSize){
    uint16_t pduLen;
    int numRecv;
    /*receving the length first*/
    if(recv(socketNumber, &pduLen, (sizeof(uint16_t)), MSG_WAITALL) < 0){
        perror("recv error ");
    }
    if(pduLen == 0){
        /*Connection was closed on the other side*/
        return 0;
    }else if (pduLen > bufferSize) {
        printf("Buffer too small to receive PDU (Missing %d bytes)", (pduLen - bufferSize));
        return 0;
    }
    
    /*Receiving the rest of the PDU*/
    numRecv = recv(socketNumber, dataBuffer, (size_t)pduLen, MSG_WAITALL);
    if(numRecv < 0){
        perror("recv error");
    }

    return numRecv;

}