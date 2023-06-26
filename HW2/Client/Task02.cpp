// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#define BUFF_SIZE 1024

//Convert string to int 
int string_to_int(char* str);

//Get message from server and print to console
void get_result(char buff[]);

int main(int argc, char* argv[])
{

	//-----------------------------------------------------
	//Step 1: Inittiate WinSock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData)) {
		printf("Version is not supported.\n");
	}


	//-----------------------------------------------------
	//Step 2: Construct socket
	SOCKET client;
	client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	//(optional) Set time-out for receiving
	int tv = 10000; //Time-out interval: 10000ms
	setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (const char*)(&tv), sizeof(int));


	//-----------------------------------------------------
	//Step 3: Input server port to bind
	char* SERVER_ADDR = argv[1];
	char* port = argv[2];
	if (port == NULL) {
		printf("Can't find port.");
		return 0;
	}
	int SERVER_PORT = string_to_int(port);

	//-----------------------------------------------------
	//Step 4:  Bind address to socket
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);


	//-----------------------------------------------------
	//Step 5: Communicate with server
	char buff[BUFF_SIZE];
	int ret, serverAddrLen = sizeof(serverAddr);

	do {
		//Send message
		gets_s(buff, BUFF_SIZE);
		if (strlen(buff) == 0) {
			closesocket(client);
			WSACleanup();
			return 0;
		}
		ret = sendto(client, buff, strlen(buff), 0, (sockaddr*)&serverAddr, serverAddrLen);
		if (ret == SOCKET_ERROR) {
			printf("Error! Cannot send mesage.");
		}

		//Receive message
		ret = recvfrom(client, buff, BUFF_SIZE, 0, (sockaddr*)&serverAddr, &serverAddrLen);

		//Display result
		if (ret == SOCKET_ERROR) {
			if (WSAGetLastError() == WSAETIMEDOUT) {
				printf("Time-out!");
			}
			else {
				printf("Error! Cannot receive message.");
			}
		}
		else {
			buff[ret] = '\0';
			get_result(buff);
		}
	} while (strcmp(buff, " ") != 0);



	//-----------------------------------------------------
	//Step6: Clean up and exit
	closesocket(client);
	WSACleanup();
	return 0;
}

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

//Get message from server and print to console
//if success, prefix is '0'
//if error, prefix is '1'
void get_result(char buff[]) {
	int i;
	if (buff[0] == '0') {
		for (i = 2; i < strlen(buff); i++) {
			printf("%c", buff[i]);
		}
	}
	else if (buff[0] == '1') {
		printf("Not found information.\n\n");
	}
}
