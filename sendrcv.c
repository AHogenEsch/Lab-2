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



int sendPDU(int clientSocket, uint8_t *dataBuffer, int lengthOfData){
    uint8_t *sendPDU = (uint8_t *)malloc(sizeof(uint8_t) * (lengthOfData + 2));
    uint16_t pduLen = htons(lengthOfData);
    int numSent;

    printf("Sending a PDU length of: %d", pduLen);
    /*add the length to the begginning of the pdu*/
    memcpy(sendPDU, &pduLen, sizeof(uint16_t));


    /*Copying the databuffer*/
    memcpy(sendPDU, dataBuffer + (sizeof(uint16_t)), sizeof(uint8_t) * lengthOfData);

    /*Sending*/
    numSent = send(clientSocket, sendPDU, lengthOfData + 2, 0);
    if(numSent < 0){
        /*Error*/
        perror("send error ");
    }

    return numSent;
}
int recvPDU(int socketNumber, uint8_t *dataBuffer, int bufferSize){
    uint16_t pduLen;
    int numRecv;
    /*receving the length first*/
    if(recv(socketNumber, &pduLen, 2, MSG_WAITALL) < 0){
        perror("recv error ");
    }
    pduLen = ntohs(pduLen);
    printf("pduLen= %d, BufferSize = %d\n", ntohs(pduLen), ntohs(bufferSize));
    if(pduLen == 0){
        /*Connection was closed on the other side*/
        return 0;
    }else if (pduLen > ntohs(bufferSize)) {
        printf("Buffer too small to receive PDU (Missing %d bytes)", (ntohs(pduLen) - htons(bufferSize)));
        return 0;
    }
    
    /*Receiving the rest of the PDU*/
    numRecv = recv(socketNumber, dataBuffer, (size_t)htons(pduLen), MSG_WAITALL);
    if(numRecv < 0){
        perror("recv error");
    }

    return numRecv;

}