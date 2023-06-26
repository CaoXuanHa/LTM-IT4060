// Task1_Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Resources.h"
#include "Bussiness.h"
#include "DataIO.h"

//USE MUTEX IN FUNTION S_Bussiness::checkAccount()

std::vector<ACCOUNT> list;

unsigned __stdcall echoThread(void* param);


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
	if (listen(listenSock, MAX_CLIENT)) {
		printf("Error! Cannot listen.\n");
		return 0;
	}
	printf("Server started!\n");

	//Step 5: Communicate with client
	sockaddr_in clientAddr;
	int clientAddrLen = sizeof(clientAddr);
	SOCKET connSock;

	DataIO::loadDataFromText(list);

	while (true) {
		connSock = accept(listenSock, (sockaddr*)&clientAddr, &clientAddrLen);
		std::cout << "Client " << connSock << " connect to server from [%s:%d] " << inet_ntoa(clientAddr.sin_addr) << " " << ntohs(clientAddr.sin_port) << std::endl;
		_beginthreadex(0, 0, echoThread, (void*)connSock, 0, 0);

	}


	//Update and reset information about client
	DataIO::updateFile(list);
	DataIO::resetAll(list);

	closesocket(connSock);
	//Step 5: Close socket
	closesocket(listenSock);
	//Step 6: Terminate Winsock
	WSACleanup();

	return 0;
}

unsigned __stdcall echoThread(void* param) {

	ACCOUNT acc = { NULL };
	MESSAGE msg = { NULL };
	int ret, status;
	SOCKET connSock = (SOCKET)param;

	while (true) {
		//Receive message from client
		ret = DataIO::receiveMessage(connSock, msg);
		if (ret > 0) {
			switch (msg.header) {
				//LOGIN type
				case LOGIN:
					acc = Bussiness::getIdAndPass(msg);
					if (acc.id != NULL) {
						status = Bussiness::checkAccount(connSock, list, acc, LOGIN);
					} else {
						status = WRONG_INPUT;
					}
					break;
					//LOGOUT type
				case LOGOUT:
					status = Bussiness::checkAccount(connSock, list, acc, LOGOUT);
					break;
					//EXIT type
				case EXIT:
					DataIO::resetAccount(connSock, list);
					status = 0;
					break;
				default:
					status = -1;
					break;
			}

			//Check status and send message back to client
			if (status > 0) {

				ret = DataIO::sendMessage(connSock, msg, status);
				if (ret == SOCKET_ERROR) {
					std::cout << "Error : " << WSAGetLastError() << std::endl;
					break;
				}

				//Receive Exit message when client want to disconnect
			} else if (status == 0) {
				std::cout << "Client " << connSock << " disconnected. " << std::endl;
				break;
			} else {
				std::cout << "Error : " << WSAGetLastError() << std::endl;
				break;
			}
		} else {
			if (WSAGetLastError() == 10054) {
				std::cout << "Client " << connSock << " disconnected. " << std::endl;
			} else {
				std::cout << "Error : " << WSAGetLastError() << std::endl;
			}
			break;
		}
	}

	DataIO::resetAccount(connSock, list);
	closesocket(connSock);
	return 0;
}
