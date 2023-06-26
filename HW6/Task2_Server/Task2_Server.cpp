// Task2_Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#undef UNICODE
#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#pragma comment (lib, "ws2_32.lib")
#pragma warning(disable:4996)

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5500
#define BUFF_SIZE 2048
#define MAX_CLIENT 1024
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

void craftMessage(Message& msg, int opcode, char* payload, int length);

int receiveData(SOCKET connSock, char* buff, int size, int flag);

int sendData(SOCKET connSock, char* buff, int size, int flag, int type);

int checkAddres(char* buff);

int getIP(char* buff);

int getName(char* buff);


int main(int argc, char* argv[]) {

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
	serverAddr.sin_port = htons((unsigned short)atoi((char*)argv[1]));
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

	if (bind(listenSock, (sockaddr*)&serverAddr, sizeof(serverAddr))) {
		printf("Error! Cannot bind this address.\n");
		_getch();
		return 0;
	}

	//Step 4: Listen request from client
	if (listen(listenSock, MAX_CLIENT)) {
		printf("Error! Cannot listen.\n");
		_getch();
		return 0;
	}

	printf("Server started!\n");

	//Step 5: Communicate with client
	SOCKET client[FD_SETSIZE], connSock;
	fd_set readfds, writefds;

	sockaddr_in clientAddr;
	int ret, nEvents, clientAddrLen;

	int result;
	char* buff = (char*)malloc(sizeof(char) * BUFF_SIZE);
	memset(buff, 0, BUFF_SIZE);

	for (int i = 0; i < FD_SETSIZE; i++) {
		client[i] = 0;
	}
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	timeval timeoutInterval;

	timeoutInterval.tv_sec = 10;
	timeoutInterval.tv_usec = 0;

	while (1) {

		FD_SET(listenSock, &readfds);
		for (int i = 0; i < FD_SETSIZE; i++) {
			if (client[i] > 0) {
				FD_SET(client[i], &readfds);
			}
		}
		writefds = readfds;

		nEvents = select(0, &readfds, 0, 0, 0);
		if (nEvents < 0) {
			printf("\nError! Cannoy poll socket: %d", WSAGetLastError());
			break;
		}
		if (FD_ISSET(listenSock, &readfds)) {
			clientAddrLen = sizeof(clientAddr);
			connSock = accept(listenSock, (sockaddr*)&clientAddr, &clientAddrLen);
			printf("Client %d connect to server\n", connSock);
			int i;
			for (i = 0; i < FD_SETSIZE; i++) {
				if (client[i] <= 0) {
					client[i] = connSock;
					break;
				}
			}
			if (i == FD_SETSIZE) {
				printf("Too much client.\n");
			}
			if (--nEvents <= 0) {
				continue;
			}
		}

		for (int i = 0; i < FD_SETSIZE; i++) {
			if (client[i] <= 0) {
				continue;
			}
			if (FD_ISSET(client[i], &readfds)) {
				ret = receiveData(client[i], buff, BUFF_SIZE, 0);
				if (ret <= 0) {
					printf("Client %d disconnected.\n", client[i]);
					FD_CLR(client[i], &readfds);
					closesocket(client[i]);
					client[i] = 0;

				} else if (ret > 0) {
					if (checkAddres(buff) == IP) {
						result = getName(buff);
					} else {
						result = getIP(buff);
					}
					ret = sendData(client[i], buff, strlen(buff), 0, result);
					if (ret == SOCKET_ERROR) {
						printf("Error! Cannot send mesage.\n");
					}
				}
			}
			if (--nEvents <= 0) {
				continue;
			}

		}
	} //end accepting

	  //Step 6: Terminate Winsock
	WSACleanup();
	_getch();
	return 0;
}


/*
* Function : craftMessage()
* - Craft data to message to send to client
* Parameter :
* - Message& msg : message that contain data
* - int opcode : opcode of message
* - char* payload : data that send to client
* - int length : length of data that send to client
* Return : void
*/
void craftMessage(Message& msg, int opcode, char* payload, int length) {
	if (opcode != NOT_FOUND) {
		msg.opcode = opcode;
		msg.length = length;
		memcpy(msg.payload, payload, length);
	} else {
		msg.opcode = opcode;
		msg.length = 0;
		memset(msg.payload, 0, MAX_PAYLOAD);
	}
}

/*
* Function : receiveData()
* - Receive data from client
* Parameter :
* - SOCKET connSock : number socket of current client
* - char* buff : buffer contain data
* - int size : size of data that send to client
* - int flag : number of flag
* Return : int
* - value return from process receiving
*/
int receiveData(SOCKET connSock, char* buff, int size, int flag) {

	Message msg;
	int ret;
	int idx = 0;

	do {
		ret = recv(connSock, (char*)&msg, sizeof(Message), flag);
		if (ret > 0) {

			memcpy(&buff[idx], msg.payload, msg.length);
			idx += msg.length;

		} else {
			return ret;
		}
	} while (msg.length != 0);

	buff[idx] = 0;
	printf("Message from client %d : %s\n", connSock, buff);
	return ret;
}


/*
* Function : sendData()
* - Send data to client
* Parameter :
* - SOCKET connSock : number socket of current client
* - char* buff : buffer contain data
* - int size : size of data that send to client
* - int flag : number of flag
* - int type : type of message that response to client's request
* Return : int
* - value return from process sending
*/
int sendData(SOCKET connSock, char* buff, int size, int flag, int type) {
	Message msg;
	int ret, length, idx = 0;

	do {
		length = (MAX_PAYLOAD < size) ? MAX_PAYLOAD : size;
		craftMessage(msg, type, &buff[idx], length);

		ret = send(connSock, (char*)&msg, sizeof(Message), flag);
		if (ret <= 0) {
			return ret;
		}
		size -= length;
		idx += length;
	} while (size != 0);

	//Notify client that server done sending
	craftMessage(msg, type, NULL, 0);
	ret = send(connSock, (char*)&msg, sizeof(Message), flag);
	if (ret <= 0) {
		return ret;
	}
	return ret;
}


/*
* Function : checkAddres()
* - Check address is IP or not
* Parameter :
* - char* buff : address that need to check
* Return : int
* - IP if address is IP or NAME if address is name
*/
int checkAddres(char* buff) {
	for (int i = 0; i < strlen(buff); i++) {
		if (buff[i] != '.' && (buff[i] < 48 || buff[i] > 57)) {
			return NAME;
		}
	}
	return IP;
}


/*
* Function : getIP()
* - Get list of IP if address is name
* Parameter :
* - char* buff : name address
* Return : int
* - IP if host != NULL and NOT FOUND if host == NULL
*/
int getIP(char* buff) {

	char* temp = (char*)malloc(sizeof(char) * BUFF_SIZE);
	memcpy(temp, buff, strlen(buff) + 1);

	struct hostent* host;
	struct in_addr addr;
	host = gethostbyname(temp);

	if (host != NULL) {

		strcpy(buff, inet_ntoa(*((struct in_addr*)host->h_addr)));

		int i = 1;
		if (host->h_addr_list[i] != NULL) {

			while (host->h_addr_list[i] != NULL) {
				addr.s_addr = *(u_long*)host->h_addr_list[i++];

				strcat(buff, " ");
				strcat(buff, inet_ntoa(addr));
			}
		}
	} else {
		return NOT_FOUND;
	}
	return IP;
}



/*
* Function : getName()
* - Get list of name if address is IP
* Parameter :
* - char* buff : IP address
* Return : int
* - NAME if host != NULL and NOT FOUND if host == NULL
*/
int getName(char* buff) {

	char* temp = (char*)malloc(sizeof(char) * BUFF_SIZE);
	memcpy(temp, buff, strlen(buff) + 1);

	struct hostent* host;
	struct in_addr addr;

	addr.s_addr = inet_addr(temp);
	host = gethostbyaddr((char*)&addr, 4, AF_INET);

	if (host != NULL) {
		strcpy(buff, host->h_name);

		if (host->h_aliases != NULL) {
			char** pAlias;
			for (pAlias = host->h_aliases; *pAlias != NULL; pAlias++) {

				strcat(buff, " ");
				strcat(buff, *pAlias);
			}
		}
	} else {
		return NOT_FOUND;
	}
	return NAME;
}
