#pragma once

#include "stdafx.h"
#include "Resources.h"

namespace DataIO {

	//Craft message to send to server
	int craftMessage(MESSAGE& msg, int type, char* message, int length) {

		switch (type) {

			case LOGIN:

				msg.header = LOGIN;
				msg.length = length + 1;
				strncpy(msg.payload, message, length);

				break;
			case LOGOUT:

				msg.header = LOGOUT;
				msg.length = 0;

				break;
			case EXIT:
				msg.header = EXIT;
				msg.length = 0;

				break;
			default:
				return -1;
		}
		return 0;
	}

	//Send message to server
	int sendMessage(SOCKET client, char* message, int type) {
		MESSAGE msg;
		int ret;

		if (type == LOGIN) {

			craftMessage(msg, LOGIN, message, strlen(message));
			ret = send(client, (char*)&msg, sizeof(msg), 0);
			if (ret == SOCKET_ERROR) {
				return -1;
			}

		} else if (type == LOGOUT) {
			craftMessage(msg, LOGOUT, NULL, 0);
			ret = send(client, (char*)&msg, sizeof(msg), 0);
			if (ret == SOCKET_ERROR) {
				return -1;
			}

		} else if (type == EXIT) {

			craftMessage(msg, EXIT, NULL, 0);
			ret = send(client, (char*)&msg, sizeof(msg), 0);
			if (ret == SOCKET_ERROR) {
				return -1;
			}

		} else {
			return -1;
		}
	}

	//Receive message from server
	int recvMessage(SOCKET client, MESSAGE& msg) {

		int ret = recv(client, (char*)&msg, sizeof(msg), 0);
		if (ret == SOCKET_ERROR) {
			return -1;
		}
		return ret;
	}
}