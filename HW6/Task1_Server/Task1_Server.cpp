// Task1_Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Resources.h"
#include "Bussiness.h"
#include "DataIO.h"

std::vector<ACCOUNT> list;

int main(int argc, char* argv[]) {
	//Step 1: Initiate WinSock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData))
		cout << "Version is not supported" << endl;

	//Step 2: Construct socket	
	SOCKET listenSock;
	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//Step 3: Bind address to socket
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons((unsigned short)atoi((char*)argv[1]));
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

	if (bind(listenSock, (sockaddr*)&serverAddr, sizeof(serverAddr))) {
		cout << "Error! Cannot bind this address." << endl;
		return 0;
	}

	//Step 4: Listen request from client
	if (listen(listenSock, MAX_CLIENT)) {
		cout << "Error! Cannot listen." << endl;
		return 0;
	}
	cout << "Server started!" << endl;


	//Step 5: Communicate with client
	SOCKET client[FD_SETSIZE], connSock;
	fd_set readfds, writefds;

	sockaddr_in clientAddr;
	int ret, nEvents, clientAddrLen;

	MESSAGE msg[FD_SETSIZE] = { NULL };
	ACCOUNT acc[FD_SETSIZE] = { NULL };
	int status[FD_SETSIZE] = { 0 };

	DataIO::loadDataFromText(list);

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
			cout << "\nError! Cannoy poll socket: " << WSAGetLastError() << endl;
			break;
		}
		if (FD_ISSET(listenSock, &readfds)) {
			clientAddrLen = sizeof(clientAddr);
			connSock = accept(listenSock, (sockaddr*)&clientAddr, &clientAddrLen);
			cout << "Client connect at socket " << connSock << endl;
			int i;
			for (i = 0; i < FD_SETSIZE; i++) {
				if (client[i] <= 0) {
					client[i] = connSock;
					break;
				}
			}
			if (i == FD_SETSIZE) {
				cout << "Too much client." << endl;
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

				ret = DataIO::receiveMessage(client[i], msg[i]);
				if (ret > 0) {
					switch (msg[i].header) {
						//LOGIN type
						case LOGIN:
							acc[i] = Bussiness::getIdAndPass(msg[i]);
							if (acc[i].id != NULL) {
								status[i] = Bussiness::checkAccount(client[i], list, acc[i], LOGIN);
							} else {
								status[i] = WRONG_INPUT;
							}
							break;
						//LOGOUT type
						case LOGOUT:
							status[i] = Bussiness::checkAccount(client[i], list, acc[i], LOGOUT);
							break;
					}

					ret = DataIO::sendMessage(client[i], msg[i], status[i]);
					if (ret <= 0) {
						cout << "Client disconnected at socket " << client[i] << endl;
						DataIO::resetAccount(client[i], list);
						FD_CLR(client[i], &readfds);
						closesocket(client[i]);
						client[i] = 0;
					}

				} else {
					cout << "Client disconnected at socket " << client[i] << endl;
					DataIO::resetAccount(client[i], list);
					FD_CLR(client[i], &readfds);
					closesocket(client[i]);
					client[i] = 0;
				}
			}
			if (--nEvents <= 0) {
				continue;
			}

		}
	} //end accepting


	//Update and reset information about client
	DataIO::updateFile(list);
	DataIO::resetAll(list);

	//Step 6: Terminate Winsock
	WSACleanup();
	_getch();

	return 0;
}

