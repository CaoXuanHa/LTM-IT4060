
#include "stdafx.h"
#include "Resources.h"
#include "DataIO.h"

int main(int argc, char* argv[]) {

	//Step 1: Inittiate WinSock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData))
		cout << "Version is not supported\n";

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
		cout << " => ERROR : Cannot connect server. " << WSAGetLastError() << endl;
		return 0;
	}

	//====================================================================

	int ret = 0;
	int count = 0;
	int status = 0;
	int key, option;
	char* filename = (char*)malloc(sizeof(char) * BUFF_SIZE);
	char* fileResult;
	do {
		system("cls");
		cout << "***************************************" << endl;
		cout << "*    Programmed by Duong Xuan Hoan    *" << endl;
		cout << "*                                     *" << endl;
		cout << "*------------[ WELCOME ]--------------*" << endl;
		cout << "*                                     *" << endl;
		cout << "*    1. Encrypt                       *" << endl;
		cout << "*    2. Decrypt                       *" << endl;
		cout << "*                                     *" << endl;

		do {
			cout << "---------------------------------------" << endl;
			//ENTER NAME OF FILE
			cout << "Enter the name of file : ";
			cin.getline(filename, BUFF_SIZE);
			if (filename[0] == 0) {
				status = EMPTY;
				break;
			}

			if (checkFile(filename)) {

				//CHOOSE OPTION : ENCRYPT/DECRYPT
				cout << "Enter option : ";
				cin >> option;
				cin.ignore(100, '\n');
				if (option != ENCRYPT && option != DECRYPT) {
					cout << " => ERROR : Wrong input." << endl;
					system("pause");
					continue;
				}

				//ENTER KEY
				cout << "Enter key : ";
				cin >> key;
				cin.ignore(100, '\n');

				//SEND REQUEST ENCRYPT/DECRYPT
				ret = sendResquest(client, option, filename, key);
				if (ret != CORRUPTED) {

					//SEND DATA OF FILE
					ret = sendFile(client, filename);
					if (ret != CORRUPTED) {

						//RECEIVE FILE RESULT FROM SERVER
						ret = recvFile(client, filename, fileResult);

						if (ret == CORRUPTED) {
							count++;
							break;
						}
					} else {
						count++;
						break;
					}
					cout << " => SYSTEM : Progress is complete !!!" << endl;
				} else {
					count++;
					break;
				}
			} else {
				cout << " => ERROR : File not found." << endl;
			}
			//printf("--------------------------------------\n");
			count++;
		} while (count != 3 && status != EMPTY);

		//IF COUNT = 3 THEN CLEAR SCREEN
		if (count == 3) {
			count = 0;
			system("pause");
		}
	} while (status != EMPTY);

	cout << "Goodbye !!!" << endl;
	free(filename);
	//====================================================================
	//Step 6: Close socket
	closesocket(client);

	//Step 7: Terminate Winsock
	WSACleanup();

	return 0;
}


