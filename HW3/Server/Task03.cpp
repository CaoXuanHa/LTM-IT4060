// TCPServer.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib,"Ws2_32.lib")
#pragma warning(disable: 4996)
#define SERVER_ADDR "127.0.0.1"
#define BUFF_SIZE 20
#define LARGE_BUFF 99999

#define OK "OK"
#undef ERROR
#define ERROR "ER"
#define LENGTH "LE"
#define MESSAGE "ME"
#define DELIMETER "|"
#define END "\n"

//Convert string to int 
int string_to_int(char* str) {
	int temp = 0;
	while (*str >= '0' && *str <= '9')
	{
		temp *= 10;
		temp += (int)(*str - '0');
		str++;
	}
	return temp;
}

//Return number of word
int wordCount(char* result) {
	int count = 0;
	if (result[0] != ' ') {
		count = 1;
	}
	for (int i = 0; i < strlen(result); i++) {
		if (result[i] > 47 && result[i] < 58) {
			return -1;
		}
		else if (result[i] == ' ' && ((result[i + 1] != ' ') && (result[i + 1] != '\0'))) {
			count++;
		}
	}
	return count;
}

//Get data from message received from client
int getMessage(char* data) {

	char* header = (char*)malloc(2 * sizeof(char));
	char* temp = (char*)malloc(LARGE_BUFF * sizeof(char));

	//Split header to get data
	strcpy(temp, data);
	header = strtok_s(temp, "|", &temp);
	temp = strtok_s(temp, "\n", &temp);

	//If message contains length of data
	if (strcmp(header, LENGTH) == 0 && atoi(temp) != 5) {
		return atoi(temp);
	}
	//If message contains data
	else if (strcmp(header, MESSAGE) == 0) {
		//If message is valid
		if (wordCount(temp) != -1) {
			return wordCount(temp);
		}
		//If message contains number
		else {
			return -1;
		}
	}
	//If message is empty
	else if (atoi(temp) == 5) {
		return 0;
	}
}

//Set message to send back to client
char* setMessage(char* data, const char* type) {
	char* temp = (char*)malloc(LARGE_BUFF * sizeof(char));

	//If message is valid
	if (strcmp(type, OK) == 0) {
		strcpy(temp, OK);
		strcat(temp, DELIMETER);
		strcat(temp, "Number of words : ");
		strcat(temp, data);
		strcat(temp, END);
		return temp;
	}
	//If message contains number
	else if (strcmp(type, ERROR) == 0) {
		strcpy(temp, ERROR);
		strcat(temp, DELIMETER);
		strcat(temp, "Error: String contains number.");
		strcat(temp, END);
		return temp;
	}
}

//Receive message from client
int receiveMessage(SOCKET connSock, char*& buff, char*& data) {
	int ret;
	int nLeft;
	int idx = 0;

	//Get message contained the length of the data
	ret = recv(connSock, buff, BUFF_SIZE, 0);
	if (ret == SOCKET_ERROR) {
		return -1;
	}
	buff[ret] = '\0';
	nLeft = getMessage(buff);

	//Get message contained data
	if (nLeft > 0) {

		while (nLeft > 0) {
			ret = recv(connSock, (char*)&buff[idx], nLeft, 0);
			if (ret == SOCKET_ERROR) {
				return -1;
			}
			idx += ret;
			nLeft -= ret;
		}
		strcpy(data, buff);
		return 1;
	}
	//If message contains number
	else if (nLeft == -1) {
		return -1;
	}
	//If message is empty
	else {
		return 0;
	}
}

//Send message to client
int sendMessage(SOCKET connSock, char*& buff, const char* type) {
	int ret;

	//If message is valid
	if (strcmp(type, OK) == 0) {
		buff = setMessage(buff, OK);
	}
	//If message contain number
	else if (strcmp(type, ERROR) == 0) {
		buff = setMessage(NULL, ERROR);
	}

	ret = send(connSock, buff, strlen(buff), 0);
	if (ret == SOCKET_ERROR) {
		return -1;
	}
	printf("%s", buff);
	return ret;
}


int main(int argc, char* argv[])
{
	//Step 1: Initiate WinSock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData))
		printf("Version is not supported\n");

	//Step 2: Construct socket	
	SOCKET listenSock;
	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//Step 3: Bind address to socket
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(string_to_int(argv[1]));
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

	if (bind(listenSock, (sockaddr*)&serverAddr, sizeof(serverAddr)))
	{
		printf("Error! Cannot bind this address.\n");
		_getch();
		return 0;
	}

	//Step 4: Listen request from client
	if (listen(listenSock, 10)) {
		printf("Error! Cannot listen.\n");
		_getch();
		return 0;
	}
	printf("Server started!\n");

	//Step 5: Communicate with client
	sockaddr_in clientAddr;
	int ret, clientAddrLen = sizeof(clientAddr);
	char* buff = (char*)malloc(BUFF_SIZE * sizeof(char));
	char* data = (char*)malloc(LARGE_BUFF * sizeof(char));

	SOCKET connSock;
	connSock = accept(listenSock, (sockaddr*)&clientAddr, &clientAddrLen);

	while (1) {
		//Receive message from client
		ret = receiveMessage(connSock, buff, data);
		if (ret == -1) {
			printf("Error : %d", WSAGetLastError());
			break;
		}
		else if (ret == 0) {
			printf("Client closed connection.\n");
		}
		else {
			//Send message to client
			int x = getMessage(buff);
			if (x != -1) {
				data = _itoa(x, data, 10);
				ret = sendMessage(connSock, data, OK);

				if (ret == SOCKET_ERROR) {
					printf("Error: %d", WSAGetLastError());
				}
			}
			else {
				data = NULL;
				ret = sendMessage(connSock, data, ERROR);

				if (ret == SOCKET_ERROR) {
					printf("Error: %d", WSAGetLastError());
				}
			}
		}
	} //end accepting



	closesocket(connSock);
	//Step 5: Close socket
	closesocket(listenSock);

	//Step 6: Terminate Winsock
	WSACleanup();

	return 0;
}
