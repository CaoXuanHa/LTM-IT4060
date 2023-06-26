// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma warning(disable: 4996)
#pragma comment(lib,"Ws2_32.lib")
#define BUFF_SIZE 20
#define LARGE_BUFF 99999


#define OK "OK"
#undef ERROR
#define ERROR "ER"
#define LENGTH "LE"
#define MESSAGE "ME"
#define DELIMETER "|"
#define END "\n"


//Set message to send to server
char* setMessage(char* data, const char* type) {
	char* temp = (char*)malloc((LARGE_BUFF + 5) * sizeof(char));
	char* dataLength = (char*)malloc(10 * sizeof(char));
	dataLength = _itoa(strlen(data) + 5, dataLength, 10);

	//Set message to send the length of string
	if (strcmp(type, LENGTH) == 0) {
		strcpy(temp, LENGTH);
		strcat(temp, DELIMETER);
		strcat(temp, dataLength);
		strcat(temp, END);
		return temp;
	}
	//Set message to send the string
	else if (strcmp(type, MESSAGE) == 0) {
		strcpy(temp, MESSAGE);
		strcat(temp, DELIMETER);
		strcat(temp, data);
		strcat(temp, END);
		return temp;
	}
}

//Get data from message received from server
char* getMessage(char*& data) {

	char* header = (char*)malloc(2 * sizeof(char));
	header = strtok_s(data, "|", &data);
	data = strtok_s(data, "\n", &data);

	//If message is valid
	if (strcmp(header, OK) == 0) {
		return data;

	}
	//If message contains number
	else if (strcmp(header, ERROR) == 0) {
		return data;
	}
	//If message is invalid
	else {
		strcpy(data, "Error message.\n");
		return data;
	}
}

//Wrapped funtion: Send message to server
int sendMessage(SOCKET client, char* data, char*& buff) {
	int ret;
	int idx = 0;
	int nLeft = strlen(data) + 5;

	//Send message contain the length of string
	buff = setMessage(data, LENGTH);
	ret = send(client, buff, strlen(buff), 0);
	if (ret == SOCKET_ERROR) {
		return -1;
	}
	//Send message contain the string
	buff = setMessage(data, MESSAGE);
	while (nLeft > 5) {
		ret = send(client, &buff[idx], nLeft, 0);
		if (ret == SOCKET_ERROR) {
			return -1;
		}
		nLeft -= ret;
		idx += ret;
	}
	return 1;
}

//Wrapped funtion: Receive message from server
int recvMessage(SOCKET client, char*& data) {
	int ret;
	ret = recv(client, data, LARGE_BUFF, 0);
	if (ret == SOCKET_ERROR) {
		return -1;
	}
	data[ret] = '\0';

	//Cut the header to get data
	data = getMessage(data);
	printf("%s\n", data);
	return ret;
}

int main(int argc, char* argv[])
{
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
	serverAddr.sin_port = htons(atoi(argv[2]));
	serverAddr.sin_addr.s_addr = inet_addr(argv[1]);

	//Step 4: Request to connect server
	if (connect(client, (sockaddr*)&serverAddr, sizeof(serverAddr))) {
		printf("Error! Cannot connect server. %d", WSAGetLastError());
		_getch();
		return 0;
	}

	//Step 5: Communicate with server
	int ret;
	char* buff = (char*)malloc(BUFF_SIZE * sizeof(char));
	char* data = (char*)malloc(LARGE_BUFF * sizeof(char));

	do {

		//Get string
		gets_s(data, LARGE_BUFF);

		//Send message to server
		if (strcmp(data, "") != 0) {
			ret = sendMessage(client, data, buff);
			if (ret == SOCKET_ERROR) {
				printf("Error! Cannot send mesage.");
			}

			//Receive echo message
			ret = recvMessage(client, data);
			if (ret == SOCKET_ERROR) {
				if (WSAGetLastError() == WSAETIMEDOUT)
					printf("Time-out!");
				else printf("Error! Cannot receive message.");
			}
		}
		else {
			ret = sendMessage(client, data, buff);
			if (ret == SOCKET_ERROR) {
				printf("Error! Cannot send mesage.");
			}
		}
	} while (strcmp(data, "") != 0);

	printf("Closed connection.\n");

	//Step 6: Close socket
	closesocket(client);
	//Step 7: Terminate Winsock
	WSACleanup();
	_getch();
	return 0;
}
