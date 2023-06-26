#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <conio.h>
#include <vector>
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable:4996)

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5500
#define BUFF_SIZE 2048
#define NONE 0
#define MAX_PAYLOAD 10

enum ADDRESS {
	IP = 1,
	NAME = 2,
	NOT_FOUND = 3
};

typedef struct Message {
	int opcode;
	int length;
	char payload[MAX_PAYLOAD];
};

void craftMessage(Message&, int, char*, int);
int sendData(SOCKET, char*, int, int);
int recvData(SOCKET, char*, int, int);
void handleResult(char*, int);

int main(int argc, char* argv[]) {

	//Step 1: Inittiate WinSock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData))
		printf("Version is not supported\n");

	//Step 2: Construct socket	
	SOCKET client;
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//(optional) Set time-out for receiving
	int tv = 10000; //Time-out interval: 10000ms
	setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (const char*)(&tv), sizeof(int));

	//Step 3: Specify server address
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons((unsigned short)atoi((char*)argv[2]));
	serverAddr.sin_addr.s_addr = inet_addr(argv[1]);

	//Step 4: Request to connect server
	if (connect(client, (sockaddr*)&serverAddr, sizeof(serverAddr))) {
		printf("Error! Cannot connect server. %d\n", WSAGetLastError());
		_getch();
		return 0;
	}
	printf("Connected server!\n");

	//Step 5: Communicate with server
	char buff[BUFF_SIZE];
	int ret;

	while (true) {
		//Send message
		printf("Enter address : ");
		gets_s(buff, BUFF_SIZE);

		//If buff is empty, then break the loop and close socket
		if (strlen(buff) == 0 || strcmp(buff, " ") == 0) {
			printf("Goodbye !!!\n");
			break;
		}

		//Send address to server
		ret = sendData(client, buff, strlen(buff), 0);
		if (ret == SOCKET_ERROR) {
			printf("Error! Cannot send mesage.\n");
		}

		//Receive echo message
		ret = recvData(client, buff, BUFF_SIZE, 0);
		if (ret == IP) {
			handleResult(buff, IP);

		} else if (ret == NAME) {
			handleResult(buff, NAME);

		} else if (ret == NOT_FOUND) {
			handleResult(NULL, NOT_FOUND);

		} else if (ret == SOCKET_ERROR) {
			if (WSAGetLastError() == WSAETIMEDOUT) {
				printf("Time-out!");
			} else {
				printf("Error! Cannot receive message.\n");
			}
		} else {
			printf("Server disconnected.\n");
			break;
		}
	}

	//Step 6: Close socket
	closesocket(client);
	//Step 7: Terminate Winsock
	WSACleanup();
	_getch();
	return 0;
}


/*
* Function : handleResult()
* - Handle message receive from server and print result to console
* Parameter :
* - char* buff : buffer that contain data
* - int type : type of result : IP or NAME
* Return : void
*/
void handleResult(char* buff, int type) {
	char* address = (char*)malloc(sizeof(char) * BUFF_SIZE);
	switch (type) {

		case IP:
			printf("Official IP : ");
			address = strtok_s(buff, " ", &buff);
			printf("%s\n", address);

			if (strlen(buff) != 0) {
				printf("Alias IP : \n");

				while (strlen(buff) != 0) {
					address = strtok_s(buff, " ", &buff);
					printf("%s\n", address);
				}
			}
			printf("\n");
			break;

		case NAME:
			printf("Official name : ");
			address = strtok_s(buff, " ", &buff);
			printf("%s\n", address);

			if (strlen(buff) != 0) {
				printf("Alias name : \n");

				while (strlen(buff) != 0) {
					address = strtok_s(buff, " ", &buff);
					printf("%s\n", address);
				}
			}
			printf("\n");
			break;

		case NOT_FOUND:
			printf("Not found information.\n\n");
			break;
		default:
			printf("Cannot handle.\n");
			break;
	}
}

/*
* Function : craftMessage()
* - Craft data to message to send to server
* Parameter :
* - Message& msg : message that contain data
* - int opcode : opcode of message
* - char* payload : data that send to client
* - int length : length of data that send to server
* Return : void
*/
void craftMessage(Message& msg, int opcode, char* payload, int length) {
	msg.opcode = opcode;
	msg.length = length;
	if (payload != NULL) {
		memcpy(msg.payload, payload, length);
	}
}

/*
* Function : recvData()
* - Receive data from server
* Parameter :
* - SOCKET connSock : number socket of current client
* - char* buff : buffer contain data
* - int size : size of data that send to server
* - int flag : number of flag
* Return : int
* - value return from process receiving
*/
int recvData(SOCKET client, char* buff, int size, int flag) {
	Message msg;
	int ret;
	int idx = 0;

	do {

		ret = recv(client, (char*)&msg, sizeof(Message), flag);
		if (ret > 0) {

			ret = msg.opcode;
			memcpy(&buff[idx], msg.payload, msg.length);
			idx += msg.length;

		} else {
			return ret;
		}
	} while (msg.length != 0);

	buff[idx] = 0;
	return ret;
}

/*
* Function : sendData()
* - Send data to server
* Parameter :
* - SOCKET connSock : number socket of current client
* - char* buff : buffer contain data
* - int size : size of data that send to server
* - int flag : number of flag
* Return : int
* - value return from process sending
*/
int sendData(SOCKET client, char* buff, int size, int flag) {
	Message msg;
	int ret, length, idx = 0;

	do {
		length = (MAX_PAYLOAD < size) ? MAX_PAYLOAD : size;
		craftMessage(msg, NONE, &buff[idx], length);

		ret = send(client, (char*)&msg, sizeof(Message), flag);
		if (ret <= 0) {
			return ret;
		}
		size -= length;
		idx += length;
	} while (size != 0);

	//Notify server that client done sending
	craftMessage(msg, NONE, NULL, 0);
	ret = send(client, (char*)&msg, sizeof(Message), flag);
	if (ret <= 0) {
		return ret;
	}

	return ret;
}


