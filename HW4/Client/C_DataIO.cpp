#include "stdafx.h"
#include "C_DataIO.h"

namespace C_DataIO {
	char* craftMessage(int type, char*& message) {

		char* temp = (char*)malloc(sizeof(char) * LARGE_BUFF);
		char* header = (char*)malloc(sizeof(char) * LARGE_BUFF);
		char* dataLength = (char*)malloc(sizeof(char) * 10);

		memset(temp, '\0', LARGE_BUFF);
		memset(header, '\0', LARGE_BUFF);
		memset(dataLength, '\0', 10);


		switch (type) {
			case LENGTH:

				sprintf(header, "%d", LENGTH);
				sprintf(dataLength, "%d", strlen(message) + 5);

				strncpy(temp, header, 3);
				strncat(temp, DELIMETER, 1);
				strncat(temp, dataLength, strlen(dataLength));
				strncat(temp, END, 1);
				break;
			case LOGIN :
				sprintf(header, "%d", LOGIN);
				strncpy(temp, header, 3);
				strncat(temp, DELIMETER, 1);
				strncat(temp, message, strlen(message));
				strncat(temp, END, 1);
				break;
			case LOGOUT :
				sprintf(header, "%d", LOGOUT);
				strncpy(temp, header, 3);
				strncat(temp, DELIMETER, 1);
				strncat(temp, message, strlen(message));
				strncat(temp, END, 1);
				break;
			case EXIT :
				sprintf(header, "%d", EXIT);
				strncpy(temp, header, 3);
				strncat(temp, DELIMETER, 1);
				strncat(temp, message, strlen(message));
				strncat(temp, END, 1);
				break;
			default :
				return NULL;
		}
		return temp;
	}
	int recoverMessage(char* message) {
		char* header = (char*)malloc(sizeof(char) * LARGE_BUFF);
		char* payload = (char*)malloc(sizeof(char) * LARGE_BUFF);

		memset(header, '\0', sizeof(header));
		memset(payload, '\0', sizeof(payload));


		strncpy(payload, message, strlen(message));
		header = strtok_s(payload, DELIMETER, &payload);
		payload = strtok_s(payload, END, &payload);

		int headerCode;
		int payloadCode = 0;
		if (header != NULL && payload != NULL) {
			headerCode = atoi(header);
			payloadCode = atoi(payload);
		}

		return payloadCode;
	}

	int sendMessage(SOCKET client, char* message, int type) {
		int ret;
		int idx = 0;
		int nLeft = strlen(message) + 5;
		char* buff = (char*)malloc(sizeof(char) * SMALL_BUFF);
		memset(buff, '\0', SMALL_BUFF);

		buff = craftMessage(LENGTH, message);
		if (buff == NULL) {
			return -1;
		}
		ret = send(client, buff, strlen(buff), 0);
		memset(buff, '\0', SMALL_BUFF);
		if (type == LOGIN) {
			buff = craftMessage(LOGIN, message);
			if (buff == NULL) {
				return -1;
			}
			while (nLeft > 5) {
				ret = send(client, &buff[idx], nLeft, 0);
				if (ret == SOCKET_ERROR) {
					return -1;
				}
				nLeft -= ret;
				idx += ret;
			}
		} else if (type == LOGOUT) {
			buff = craftMessage(LOGOUT, message);
			while (nLeft > 5) {
				ret = send(client, &buff[idx], nLeft, 0);
				if (ret == SOCKET_ERROR) {
					return -1;
				}
				nLeft -= ret;
				idx += ret;
			}
		} else if (type == EXIT) {
			buff = craftMessage(EXIT, message);
			while (nLeft > 5) {
				ret = send(client, &buff[idx], nLeft, 0);
				if (ret == SOCKET_ERROR) {
					return -1;
				}
				nLeft -= ret;
				idx += ret;
			}
		} else {
			return -1;
		}
	}

	int receiveMessage(SOCKET client, char*& message) {
		int ret = recv(client, message, LARGE_BUFF, 0);
		if (ret == SOCKET_ERROR) {
			return -1;
		}
		message[ret] = 0;
		return ret;
	}
}