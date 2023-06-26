
#include "stdafx.h"
#include "S_DataIO.h"
#define stringify( name ) # name
namespace S_DataIO {

	//Add header to message
	char* craftMessage(int status) {
		int type;
		char* temp = (char*)malloc(sizeof(char) * LARGE_BUFF);
		char* header = (char*)malloc(sizeof(char) * LARGE_BUFF);
		char* payload = (char*)malloc(sizeof(char) * LARGE_BUFF);

		memset(temp, '\0', LARGE_BUFF);
		memset(header, '\0', LARGE_BUFF);
		memset(payload, '\0', LARGE_BUFF);

		if (status == LOGIN_SUCCESS || status == LOGOUT_SUCCESS) {
			type = OK;
		} else {
			type = ERR;
		}

		sprintf(header, "%d", type);
		strncpy(temp, header, 3);
		strncat(temp, DELIMETER, 1);
		sprintf(payload, "%d", status);
		strncat(temp, payload, 3);
		strncat(temp, END, 1);

		std::cout << "SEND TO CLIENT : ";
		printSendMessage(type, status);
		std::cout << "\n---------------------------------------\n";
		return temp;
	}
	/*-----------------Communicate with file-------------------------*/

		//Get data from text and fill to list user
	void loadDataFromText(ACCOUNT list[]) {
		int index = 0;
		std::ifstream file_(FILE_ACCOUNT);

		if (!file_.fail()) {
			while (!file_.eof()) {
				file_ >> list[index].id >> list[index].pass >> list[index].status;
				index++;
			}
			std::cout << "Read file complete." << std::endl;
		} else {
			std::cout << "Cannot open file" << std::endl;
		}
		file_.close();
	}

	//Reset only one client
	void resetAccount(SOCKET connSock, ACCOUNT list[]) {
		for (int i = 0; i < MAX_USER; i++) {
			if (list[i].socket == connSock) {
				list[i].socket = 0;
				list[i].wrongPassword = 0;
			}
		}
	}

	//Reset every thing about all client to 0 such as socket number, wrong password number 
	void resetAll(ACCOUNT list[]) {
		for (int i = 0; i < MAX_USER; i++) {
			list[i].socket = 0;
			list[i].wrongPassword = 0;
		}
	}

	//Update status of account to file
	void updateFile(ACCOUNT list[]) {
		std::ofstream file_(FILE_ACCOUNT);

		if (!file_.fail()) {
			for (int i = 0; i < 3; i++) {
				file_ << list[i].id << " " << list[i].pass << " " << list[i].status << std::endl;
			}
		}
		file_.close();
	}

	/*-----------------Communicate with client-------------------------*/

		//Send message to client
	int sendMessage(SOCKET connSock, char* message) {
		int ret;
		ret = send(connSock, message, strlen(message), 0);
		if (ret == SOCKET_ERROR) {
			return -1;
		}
		return ret;
	}


	//Receive message from client
	int receiveMessage(SOCKET connSock, char*& message) {
		int ret, nLeft;
		int idx = 0;
		char* buff = (char*)malloc(sizeof(char) * SMALL_BUFF);
		memset(buff, 0, SMALL_BUFF);

		//Get length of the message
		ret = recv(connSock, buff, SMALL_BUFF, 0);
		if (ret == SOCKET_ERROR) {
			return -1;
		}
		buff[ret] = 0;

		//Receive byte stream from client
		int length = getLength(buff);
		nLeft = length;

		if (nLeft != -1) {
			while (nLeft > 0) {
				ret = recv(connSock, (char*)&buff[idx], nLeft, 0);
				if (ret == SOCKET_ERROR) {
					return -1;
				}
				idx += ret;
				nLeft -= ret;
			}
			strncpy(message, buff, length);
			return ret;
		} else {
			std::cout << "Missing length of message." << std::endl;
			return -1;
		}
	}

	//Get length of message
	int getLength(char*& buff) {
		char* header = (char*)malloc(sizeof(char) * LARGE_BUFF);
		char* payload = (char*)malloc(sizeof(char) * LARGE_BUFF);

		memset(header, '\0', LARGE_BUFF);
		memset(payload, '\0', LARGE_BUFF);

		header = strtok_s(buff, DELIMETER, &buff);
		payload = strtok_s(buff, END, &buff);

		int headerCode;
		if (header != NULL) {
			headerCode = atoi(header);
			return atoi(payload);
		} else {
			headerCode = 0;
			return -1;
		}
	}

	//Print message reponse to client to sreen
	void printSendMessage(int type, int status) {
		switch (type) {
			case OK :
				std::cout << "OK";
				switch (status) {
					case LOGIN_SUCCESS :
						std::cout << " LOGIN_SUCCESS" << std::endl;
						break;
					case LOGOUT_SUCCESS:
						std::cout << " LOGOUT_SUCCESS" << std::endl;
						break;
					default :
						break;
				}
				break;
			case ERR :
				std::cout << "ERR";
				switch (status) {
					case WRONG_INPUT:
						std::cout << " WRONG_INPUT" << std::endl;
						break;
					case ID_NOT_FOUND:
						std::cout << " ID_NOT_FOUND" << std::endl;
						break;
					case WRONG_PASSWORD:
						std::cout << " ALREADY_LOGIN" << std::endl;
						break;
					case ALREADY_LOGIN:
						std::cout << " ALREADY_LOGIN" << std::endl;
						break;
					case HAVENT_LOGIN:
						std::cout << " HAVENT_LOGIN" << std::endl;
						break;
					case BLOCKED:
						std::cout << " BLOCKED" << std::endl;
						break;
					default :
						break;
				}
				break;
			default :
				break;

		}
	}
}

