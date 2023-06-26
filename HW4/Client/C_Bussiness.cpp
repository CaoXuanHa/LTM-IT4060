#include "stdafx.h"
#include "C_Bussiness.h"

namespace C_Bussiness {
	int checkInput(char* input) {
		if (strlen(input) < 20) {
			for (int i = 0; i < strlen(input); i++) {
				if (input[i] == ' ') {
					return HAS_SPACE;
				}
			}
			return VALID;
		} else {
			return TOO_MUCH;
		}
	}

	void login(SOCKET client, char id[], char pass[]) {
		char* message = (char*)malloc(LARGE_BUFF * sizeof(char));
		memset(message, '\0', LARGE_BUFF);
		strncpy(message, id, strlen(id));
		strncat(message, DELIMETER, 1);
		strncat(message, pass, strlen(pass));

		int ret = C_DataIO::sendMessage(client, message, LOGIN);
		if (ret == SOCKET_ERROR) {
			std::cout << "Error! Cannot send mesage." << std::endl;
			return;
		}
	}

	void logout(SOCKET client) {
		char* message = (char*)malloc(LARGE_BUFF * sizeof(char));
		memset(message, '\0', LARGE_BUFF);
		strncpy(message, " ", 1);

		int ret = C_DataIO::sendMessage(client, message, LOGOUT);
		if (ret == SOCKET_ERROR) {
			std::cout << "Error! Cannot send mesage." << std::endl;
			return;
		}
	}

	void end(SOCKET client) {
		char* message = (char*)malloc(LARGE_BUFF * sizeof(char));
		memset(message, '\0', LARGE_BUFF);
		strncpy(message, " ", 1);

		int ret = C_DataIO::sendMessage(client, message, EXIT);
		if (ret == SOCKET_ERROR) {
			std::cout << "Error! Cannot send mesage." << std::endl;
			return;
		}
	}

	void handlResponse(SOCKET client) {

		char* message = (char*)malloc(sizeof(char) * LARGE_BUFF);
		memset(message, '\0', LARGE_BUFF);


		int ret = C_DataIO::receiveMessage(client, message);
		if (ret == SOCKET_ERROR) {
			if (WSAGetLastError() == WSAETIMEDOUT)
				printf("Time-out!");
			else printf("Error! Cannot receive message.");
		}

		int status = C_DataIO::recoverMessage(message);

		switch (status) {
			case LOGIN_SUCCESS:
				std::cout << " => Code 200 : Login successful." << std::endl;
				break;
			case LOGOUT_SUCCESS:
				std::cout << " => Code 201 : Logout successful." << std::endl;
				break;

			case WRONG_INPUT:
				std::cout << " => Code 207 : Your input is not right." << std::endl;
				break;
			case ID_NOT_FOUND:
				std::cout << " => Code 202 : ID not found." << std::endl;
				break;
			case WRONG_PASSWORD:
				std::cout << " => Code 203 : Wrong password." << std::endl;
				break;
			case HAVENT_LOGIN:
				std::cout << " => Code 205 : You haven't login." << std::endl;
				break;
			case ALREADY_LOGIN:
				std::cout << " => Code 206 : You've already login. Please logout to login with another account." << std::endl;
				break;
			case BLOCKED:
				std::cout << " => Code 204 : Your account is blocked." << std::endl;
				break;
			default:
				std::cout << "Wrong message receive." << std::endl;
				break;
		}
	}
}