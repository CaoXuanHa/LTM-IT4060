// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "S_Resource.h"
#include "S_Bussiness.h"


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
		return 0;
	}

	//Step 4: Listen request from client
	if (listen(listenSock, 10)) {
		printf("Error! Cannot listen.\n");
		return 0;
	}
	printf("Server started!\n");

	//Step 5: Communicate with client
	sockaddr_in clientAddr;
	int clientAddrLen = sizeof(clientAddr);

	ACCOUNT list[MAX_USER] = { NULL };
	S_DataIO::loadDataFromText(list);

	ACCOUNT acc = { NULL };
	MESSAGE msg = { NULL };

	SOCKET connSock;
	connSock = accept(listenSock, (sockaddr*)&clientAddr, &clientAddrLen);
	std::cout << "Client " << connSock << " connect to server from [%s:%d] " << inet_ntoa(clientAddr.sin_addr) << " " << ntohs(clientAddr.sin_port) << std::endl;

	int ret, status, typeMessage;
	char* message = (char*)malloc(sizeof(char) * LARGE_BUFF);
	memset(message, '\0', LARGE_BUFF);


	while (true) {
		//Receive message from client
		ret = S_DataIO::receiveMessage(connSock, message);
		if (ret != -1) {

			//Check type of message and handle it for diferent way
			typeMessage = S_Bussiness::handleMessage(msg, message);
			switch (typeMessage) {
				//LOGIN type
				case LOGIN:
					acc = S_Bussiness::getIdAndPass(msg.payload);
					if (acc.id != NULL) {
						status = S_Bussiness::checkAccount(connSock, list, acc, LOGIN);
					} else {
						message = S_DataIO::craftMessage(WRONG_INPUT);
						ret = S_DataIO::sendMessage(connSock, message);
					}
					break;
					//LOGOUT type
				case LOGOUT:
					status = S_Bussiness::checkAccount(connSock, list, acc, LOGOUT);
					break;
					//EXIT type
				case EXIT:
					S_DataIO::resetAccount(connSock, list);
					status = -1;
					break;
				default:
					status = -1;
					break;
			}

			//Check status and send message back to client
			if (status != -1) {
				message = S_DataIO::craftMessage(status);
				ret = S_DataIO::sendMessage(connSock, message);
				if (ret == SOCKET_ERROR) {
					std::cout << "Error : " << WSAGetLastError() << std::endl;
					break;
				}
			} else {
				std::cout << "Error : " << WSAGetLastError() << std::endl;
				break;
			}
		} else {
			std::cout << "Error : " << WSAGetLastError() << std::endl;
			break;
		}
	}

	//Update and reset information about client
	S_DataIO::updateFile(list);
	S_DataIO::resetAll(list);

	closesocket(connSock);
	//Step 5: Close socket
	closesocket(listenSock);
	//Step 6: Terminate Winsock
	WSACleanup();

	return 0;
}

