// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "C_Resource.h"
#include "C_Bussiness.h"

void enterIdAndPass(char id[], char pass[]);

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
		printf("Error! Cannot connect server. %d", WSAGetLastError());
		return 0;
	}
	/*-------------------------------------------------------------------*/


	//Step 5: Communicate with server
	//MENU
	char id[30];
	char pass[30];
	memset(id, '\0', 30);
	memset(pass, '\0', 30);

	int choice;
	char* input = (char*)malloc(sizeof(char) * LARGE_BUFF);
	int count = 0;

	do {
		system("cls");
		std::cout << "\n**************************************";
		std::cout << "\n*    Programmed by Duong Xuan Hoan    *";
		std::cout << "\n*                                     *";
		std::cout << "\n*------------[ WELCOME ]--------------*";
		std::cout << "\n*                                     *";
		std::cout << "\n*    1. Login                         *";
		std::cout << "\n*    2. Logout                        *";
		std::cout << "\n*    3. Exit                          *";
		std::cout << "\n*                                     *";
		std::cout << "\n*-------------------------------------*\n";
		do {
			std::cout << "Please enter your selection : ";
			std::cin.getline(input, LARGE_BUFF);
			if (strlen(input) > 1 || atoi(input) == 0) {
				choice = 0;
			} else {
				choice = atoi(input);
			}

			if (choice == CHOOSE_LOGIN) {
				enterIdAndPass(id, pass);
				C_Bussiness::login(client, id, pass);
				C_Bussiness::handlResponse(client);

			} else if (choice == CHOOSE_LOGOUT) {
				C_Bussiness::logout(client);
				C_Bussiness::handlResponse(client);

			} else if (choice == CHOOSE_EXIT) {
				C_Bussiness::end(client);
				std::cout << "Goodbye." << std::endl;

			} else {
				std::cout << "WARNING : Wrong input. ";
			}
			std::cout << "\n---------------------------------------\n";
			count++;
		} while (choice != CHOOSE_EXIT && count != 5);
		count = 0;
		system("pause");
	} while (choice != CHOOSE_EXIT);


	/*-------------------------------------------------------------*/
	printf("Closed connection.\n");
	//Step 6: Close socket
	closesocket(client);
	//Step 7: Terminate Winsock
	WSACleanup();
	return 0;
}

void enterIdAndPass(char id[], char pass[]) {
	int status = 0;
	do {
		switch (status) {
			case VALID:
				break;
			case HAS_SPACE:
				std::cout << "WARNING : Username or Password only doesn't allow to have space" << std::endl;
				break;
			case TOO_MUCH:
				std::cout << "WARNING : Username or Password only has 20 character" << std::endl;
				break;
		}
		std::cout << "Username : ";
		std::cin.getline(id, LARGE_BUFF);
		std::cout << "Password : ";
		std::cin.getline(pass, LARGE_BUFF);

		if (C_Bussiness::checkInput(id) >= C_Bussiness::checkInput(pass)) {
			status = C_Bussiness::checkInput(id);
		} else {
			status = C_Bussiness::checkInput(pass);
		}
	} while (status != 0);
}
