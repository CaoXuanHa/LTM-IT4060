// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#undef UNICODE
#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma comment (lib, "ws2_32.lib")
#define SERVER_ADDR "127.0.0.1"
#define BUFF_SIZE 1024

//Convert string to int 
int string_to_int(char* str);

//Check address is IP or not
int isIP(char address[]);

//Get list of IP if address is domain name
void get_IP(char address[], char result[]);

//Get list of name if address is IP
void get_name(char address[], char result[]);

//---------------------------------------------------------
int main(int argc, char* argv[]) {

	//Step 1: Inittiate WinSock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData)) {
		printf("Version is not supported\n");
	}

	//-----------------------------------------------------
	//Step 2: Construct socket
	SOCKET server;
	server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);


	//-----------------------------------------------------
	//Step 3: Input server port to bind
	char* port = argv[1];
	if (port == NULL) {
		printf("Can't find port.");
		return 0;
	}
	int SERVER_PORT = string_to_int(port);


	//-----------------------------------------------------
	//Step 4: Bind address to socket
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	if (bind(server, (sockaddr*)&serverAddr, sizeof(serverAddr)))
	{
		printf("Error! Cannot bind this address.\n");
		return 0;
	}
	printf("Server started!\n");


	//-----------------------------------------------------
	//Step 5: Communicate with client
	sockaddr_in clientAddr;
	int ret, clientAddrLen = sizeof(clientAddr);

	char address[BUFF_SIZE];
	char result[BUFF_SIZE];

	while (1) {
		//Receive message
		ret = recvfrom(server, address, BUFF_SIZE, 0, (sockaddr*)&clientAddr, &clientAddrLen);
		if (ret == SOCKET_ERROR) {
			printf("Error : %d", WSAGetLastError());
		}
		else {
			address[ret] = '\0';
			printf("Receive from client[%s:%d] %s\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), address);


			//Address resolver function
			if (isIP(address) == 0) {
				get_name(address, result);
				ret = sendto(server, result, strlen(result), 0, (SOCKADDR*)&clientAddr, sizeof(clientAddr));
			}
			else if (isIP(address) == 1) {
				get_IP(address, result);
				ret = sendto(server, result, strlen(result), 0, (SOCKADDR*)&clientAddr, sizeof(clientAddr));
				if (ret == SOCKET_ERROR) {
					printf("Error: %d", WSAGetLastError());
				}
			}
			else {
				strcpy_s(result,BUFF_SIZE, "1");
				ret = sendto(server, result, strlen(result), 0, (SOCKADDR*)&clientAddr, sizeof(clientAddr));
				if (ret == SOCKET_ERROR) {
					printf("Error: %d", WSAGetLastError());
				}
			}
		}

	} //end while


	  //-----------------------------------------------------
	  //Step6: Clean up and exit
	closesocket(server);
	WSACleanup();
	return 0;
}
//---------------------------------------------------------

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


//Check address is IP or not
int isIP(char address[]) {
	for (int i = 0; i < strlen(address); i++) {
		if (address[i] == '.') {
			if (address[i + 1] > 47 && address[i + 1] < 58) {
				return 0;   //true
			}
			else {
				return 1;   //false
			}
		}
	}
	return -1;				//if input is not address
}

//Get list of IP if address is domain name and send message to client
//if success : 0|Official IP : 125.212.247.65\nAlias IP : \n125.212.247.245\n\n
//if error : 1
void get_IP(char address[], char result[]) {

	struct hostent* host;
	struct in_addr addr;
	host = gethostbyname(address);
	if (host != NULL) {
		strcpy_s(result, BUFF_SIZE, "0");
		strcat_s(result,BUFF_SIZE, "|Official IP : ");
		strcat_s(result, BUFF_SIZE, inet_ntoa(*((struct in_addr*)host->h_addr)));
		strcat_s(result, BUFF_SIZE, "\n");

		int i = 1;
		if (host->h_addr_list[i] != NULL) {
			strcat_s(result, BUFF_SIZE, "Alias IP : \n");
			while (host->h_addr_list[i] != NULL) {
				addr.s_addr = *(u_long*)host->h_addr_list[i++];
				strcat_s(result, BUFF_SIZE, inet_ntoa(addr));
				strcat_s(result, BUFF_SIZE, "\n");
			}
		}
		strcat_s(result, BUFF_SIZE, "\n");
	}
	else {
		strcpy_s(result, BUFF_SIZE, "1");
	}
}

//Get list of name if address is IP
//if success : 0|Official name : 24h.com.vn\nAlias name : \n24h.com.vn\n\n
//if error : 1
void get_name(char address[], char result[]) {
	struct hostent* host;
	struct in_addr addr;

	addr.s_addr = inet_addr(address);
	host = gethostbyaddr((char*)&addr, 4, AF_INET);

	if (host != NULL) {
		strcpy_s(result, BUFF_SIZE, "0");
		strcat_s(result, BUFF_SIZE, "|Official name :");
		strcat_s(result, BUFF_SIZE, host->h_name);
		strcat_s(result, BUFF_SIZE, "\n");

		if (host->h_aliases != NULL) {
			char** pAlias;
			strcat_s(result, BUFF_SIZE, "Alias name : \n");
			for (pAlias = host->h_aliases; *pAlias != NULL; pAlias++) {
				strcat_s(result, BUFF_SIZE, *pAlias);
				strcat_s(result, BUFF_SIZE, "\n");
			}
		}
		strcat_s(result, BUFF_SIZE, "\n");
	}
	else {
		strcpy_s(result, BUFF_SIZE, "1");
	}
}