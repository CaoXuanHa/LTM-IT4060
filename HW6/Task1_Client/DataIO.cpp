#include "stdafx.h"
#include "DataIO.h"

namespace DataIO {

	/*
	* Function : craftMessage()
	* - Craft message to form of struct MESSAGE before sending to server
	* Parameter :
	* - MESSAGE& msg : message that craft to send to server
	* - int type : type of message that want to send to server
	* - char* message : message that want to send to server
	* - int length : length of message
	* Return : void
	*/
	void craftMessage(MESSAGE& msg, int type, char* message, int length) {

		switch (type) {

			case LOGIN:

				msg.opcode = LOGIN;
				msg.length = length + 1;
				strncpy(msg.payload, message, length);
				break;
			case LOGOUT:

				msg.opcode = LOGOUT;
				msg.length = 0;
				break;
		}
		return;
	}

	/*
	* Function : sendMessage()
	* - Send message to server
	* Parameter :
	* - SOCKET client : number socket of current client
	* - char* message : message that want to send to server
	* - int type : type of message that want to send to server
	* Return : int
	* - Value return from process sending
	*/
	int sendMessage(SOCKET client, char* message, int type) {
		MESSAGE msg;
		int ret;

		if (type == LOGIN) {

			craftMessage(msg, LOGIN, message, strlen(message));
			ret = send(client, (char*)&msg, sizeof(msg), 0);
			if (ret == SOCKET_ERROR) {
				cout << " => ERROR : Cannot send message." << endl;
			}
			return ret;

		} else if (type == LOGOUT) {
			craftMessage(msg, LOGOUT, NULL, 0);
			ret = send(client, (char*)&msg, sizeof(msg), 0);
			if (ret == SOCKET_ERROR) {
				cout << " => ERROR : Cannot send message." << endl;
			}
			return ret;
		} else {
			return -1;
		}
	}

	/*
	* Function : recvMessage()
	* - Send message to server
	* Parameter :
	* - SOCKET client : number socket of current client
	* - MESSAGE& msg : message that receive from server
	* Return : int
	* - Value return from process receiving
	*/
	int recvMessage(SOCKET client, MESSAGE& msg) {

		int ret = recv(client, (char*)&msg, sizeof(msg), 0);
		if (ret == SOCKET_ERROR) {
			if (WSAGetLastError() == WSAETIMEDOUT)
				cout << "Time-out!" << endl;
			else {
				cout << " => ERROR : Cannot receive message." << endl;
			}
			return -1;
		}
		return ret;
	}
}