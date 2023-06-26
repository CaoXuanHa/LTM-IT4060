#include "stdafx.h"
#include "Bussiness.h"

namespace Bussiness {

	/*
	* Function : checkInput()
	* - Check input that is too much character or has space or valid
	* Parameter :
	* - char* input : input get from user
	* Return : int
	* - Value of enum INPUT_TYPE
	*/
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

	/*
	* Function : login()
	* - User send login request to server
	* Parameter :
	* - SOCKET client : number socket of current client
	* - char id[] : id or username of user
	* - char pass[] : password of user
	* Return : void
	*/
	void login(SOCKET client, char id[], char pass[]) {

		char* message = (char*)malloc(BUFF_SIZE * sizeof(char));
		memset(message, 0, BUFF_SIZE);

		strncpy(message, id, strlen(id));
		strncat(message, DELIMETER, 1);
		strncat(message, pass, strlen(pass) + 1);

		int ret = DataIO::sendMessage(client, message, LOGIN);
		return;
	}

	/*
	* Function : logout()
	* - User send logout request to server
	* Parameter :
	* - SOCKET client : number socket of current client
	* Return : void
	*/
	void logout(SOCKET client) {
		int ret = DataIO::sendMessage(client, NULL, LOGOUT);
		return;
	}

	/*
	* Function : handlResponse()
	* - Get message from server and print result to console
	* Parameter :
	* - SOCKET client : number socket of current client
	* Return : void
	*/
	void handlResponse(SOCKET client) {
		MESSAGE msg;

		int ret = DataIO::recvMessage(client, msg);
		if (ret == SOCKET_ERROR) {
			if (WSAGetLastError() == WSAETIMEDOUT)
				cout << "Time-out!" << endl;
			else {
				cout << "Error! Cannot receive message." << endl;
			}
		}

		int status = atoi(msg.payload);

		switch (status) {
			case LOGIN_SUCCESS:
				cout << " => Code 200 : Login successful." << endl;
				break;
			case LOGOUT_SUCCESS:
				cout << " => Code 201 : Logout successful." << endl;
				break;

			case WRONG_INPUT:
				cout << " => Code 301 : Your input is not right." << endl;
				break;
			case ID_NOT_FOUND:
				cout << " => Code 302 : ID not found." << endl;
				break;
			case WRONG_PASSWORD:
				cout << " => Code 303 : Wrong password." <<endl;
				break;
			case ALREADY_LOGIN:
				cout << " => Code 304 : You've already login. Please logout to login with another account." << endl;
				break;
			case CANT_LOGIN:
				cout << " => Code 305 : Your account has already login in another machine." << endl;
				break;
			case HAVENT_LOGIN:
				cout << " => Code 306 : You haven't login yet." << endl;
				break;
			case BLOCKED:
				cout << " => Code 307 : Your account is blocked." << endl;
				break;
			default:
				cout << "Wrong message." << endl;
				break;
		}
		return;
	}
}
