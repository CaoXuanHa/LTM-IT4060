#pragma once
#include "stdafx.h"
#include "Resources.h"
#include "DataIO.h"

namespace Bussiness {
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

		char* message = (char*)malloc(BUFF_SIZE * sizeof(char));
		memset(message, 0, BUFF_SIZE);

		strncpy(message, id, strlen(id));
		strncat(message, DELIMETER, 1);
		strncat(message, pass, strlen(pass) + 1);


		int ret = DataIO::sendMessage(client, message, LOGIN);
		if (ret == SOCKET_ERROR) {
			std::cout << "Error! Cannot send mesage." << std::endl;
			return;
		}
	}

	void logout(SOCKET client) {

		int ret = DataIO::sendMessage(client, NULL, LOGOUT);
		if (ret == SOCKET_ERROR) {
			std::cout << "Error! Cannot send mesage." << std::endl;
			return;
		}
	}

	void end(SOCKET client) {

		int ret = DataIO::sendMessage(client, NULL, EXIT);
		if (ret == SOCKET_ERROR) {
			std::cout << "Error! Cannot send mesage." << std::endl;
			return;
		}
	}

	void handlResponse(SOCKET client) {
		MESSAGE msg;

		int ret = DataIO::recvMessage(client, msg);

		if (ret == SOCKET_ERROR) {
			if (WSAGetLastError() == WSAETIMEDOUT)
				printf("Time-out!");
			else printf("Error! Cannot receive message.");
		}

		int status = atoi(msg.payload);

		switch (status) {
			case LOGIN_SUCCESS:
				std::cout << " => Code 200 : Login successful." << std::endl;
				break;
			case LOGOUT_SUCCESS:
				std::cout << " => Code 201 : Logout successful." << std::endl;
				break;

			case WRONG_INPUT:
				std::cout << " => Code 301 : Your input is not right." << std::endl;
				break;
			case ID_NOT_FOUND:
				std::cout << " => Code 302 : ID not found." << std::endl;
				break;
			case WRONG_PASSWORD:
				std::cout << " => Code 303 : Wrong password." << std::endl;
				break;
			case ALREADY_LOGIN:
				std::cout << " => Code 304 : You've already login. Please logout to login with another account." << std::endl;
				break;
			case CANT_LOGIN:
				std::cout << " => Code 305 : Your account has already login in another machine." << std::endl;
				break;
			case HAVENT_LOGIN:
				std::cout << " => Code 306 : You haven't login yet." << std::endl;
				break;
			case BLOCKED:
				std::cout << " => Code 307 : Your account is blocked." << std::endl;
				break;
			default:
				std::cout << "Wrong message receive." << std::endl;
				break;
		}
	}
}

