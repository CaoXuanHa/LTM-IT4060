#include "stdafx.h"
#include "S_Bussiness.h"

namespace S_Bussiness {

	//Check data in message from client is valid or not
	int checkData(char* data) {
		if (strlen(data) < 20) {
			for (int i = 0; i < strlen(data); i++) {
				if (data[i] == ' ') {
					return INVALID;
				}
			}
			return VALID;
		} else {
			return INVALID;
		}
	}

	//Check account status get from client
	int checkAccount(SOCKET connSock, ACCOUNT list[], ACCOUNT& acc, int type) {

		if (type == LOGIN) {
			for (int i = 0; i < MAX_USER; i++) {
				if (list[i].socket != 0 || list[i].socket == connSock) {
					return ALREADY_LOGIN;
				}
			}
			for (int i = 0; i < MAX_USER; i++) {

				if (strcmp(list[i].id, acc.id) == 0) {
					if (strcmp(list[i].pass, acc.pass) == 0) {
						if (list[i].status == 0) {
							if (list[i].socket == 0) {

								list[i].socket = connSock;
								acc.socket = connSock;
								return LOGIN_SUCCESS;
							}
						} else {
							return BLOCKED;
						}
					} else {

						list[i].wrongPassword++;
						if (list[i].wrongPassword > 3) {
							list[i].status = 1;
							return BLOCKED;
						} else {
							return WRONG_PASSWORD;
						}
					}
				}
			}
			return ID_NOT_FOUND;

		} else if (type == LOGOUT) {
			for (int i = 0; i < MAX_USER; i++) {
				if (list[i].socket == connSock) {
					list[i].socket = 0;
					return LOGOUT_SUCCESS;
				}
			}
			return HAVENT_LOGIN;

		} else {
			return -1;
		}
	}

	//Get id and pass from message LOGIN
	ACCOUNT getIdAndPass(char* payload) {
		ACCOUNT acc = { NULL };
		char* id = (char*)malloc(sizeof(char) * SMALL_BUFF);
		char* pass = (char*)malloc(sizeof(char) * SMALL_BUFF);

		memset(id, '\0', SMALL_BUFF);
		memset(pass, '\0', SMALL_BUFF);

		id = strtok_s(payload, DELIMETER, &payload);
		pass = strtok_s(payload, END, &payload);

		if (checkData(id) == VALID && checkData(pass) == VALID) {
			strncpy(acc.id, id, strlen(id));
			strncpy(acc.pass, pass, strlen(pass));
			return acc;
		} else {
			return { NULL };
		}

	}

	//Handle message got from client 
	int handleMessage(MESSAGE& msg, char*& buff) {

		char* header = (char*)malloc(sizeof(char) * LARGE_BUFF);
		char* payload = (char*)malloc(sizeof(char) * LARGE_BUFF);

		memset(header, '\0', LARGE_BUFF);
		memset(payload, '\0', LARGE_BUFF);

		header = strtok_s(buff, DELIMETER, &buff);
		payload = strtok_s(buff, END, &buff);

		int headerCode;
		if (header != NULL) {
			headerCode = atoi(header);
		} else {
			headerCode = 0;
		}

		switch (headerCode) {
			case LENGTH:
				return LENGTH;
			case LOGIN:
				msg.header = headerCode;
				strncpy(msg.payload, payload, strlen(payload));
				std::cout << "MESAGE TYPE : LOGIN " << msg.payload << std::endl;
				return LOGIN;
			case LOGOUT:
				std::cout << "MESAGE TYPE : LOGOUT" << std::endl;
				return LOGOUT;
			case EXIT:
				std::cout << "MESAGE TYPE : EXIT" << std::endl;
				return EXIT;
			default:
				std::cout << "MESAGE TYPE : FAIL" << std::endl;
				return -1;
		}

	}


}


