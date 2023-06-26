#include "stdafx.h"
#include "DataIO.h"

namespace DataIO {

	/*
	* Function : printSendMessage()
	* - Print message that send to client to console
	* Parameter :
	* - int type : type of message, OK or ERR
	* - int status : one of value enum STATUS_CODE
	* Return : void
	*/
	void printSendMessage(int type, int status) {
		switch (type) {
			case OK:
				std::cout << "OK";
				switch (status) {
					case LOGIN_SUCCESS:
						std::cout << " LOGIN_SUCCESS" << std::endl;
						break;
					case LOGOUT_SUCCESS:
						std::cout << " LOGOUT_SUCCESS" << std::endl;
						break;
					default:
						break;
				}
				break;
			case ERR:
				std::cout << "ERR";
				switch (status) {
					case WRONG_INPUT:
						std::cout << " WRONG_INPUT" << std::endl;
						break;
					case ID_NOT_FOUND:
						std::cout << " ID_NOT_FOUND" << std::endl;
						break;
					case WRONG_PASSWORD:
						std::cout << " WRONG_PASSWORD" << std::endl;
						break;
					case ALREADY_LOGIN:
						std::cout << " ALREADY_LOGIN" << std::endl;
						break;
					case CANT_LOGIN:
						std::cout << " CANT_LOGIN " << std::endl;
						break;
					case HAVENT_LOGIN:
						std::cout << " HAVENT_LOGIN" << std::endl;
						break;
					case BLOCKED:
						std::cout << " BLOCKED" << std::endl;
						break;
					default:
						break;
				}
				break;
			default:
				break;

		}
	}


	/*
	* Function : loadDataFromText()
	* - Load data from text to list account
	* Parameter :
	* - vector<ACCOUNT>& list : list of account
	* Return : void
	*/
	void loadDataFromText(std::vector<ACCOUNT>& list) {
		int index = 0;
		std::ifstream file_(FILE_ACCOUNT);

		if (!file_.fail()) {
			while (!file_.eof()) {
				ACCOUNT acc = { NULL };
				file_ >> acc.id >> acc.pass >> acc.status;
				list.push_back(acc);
			}
			std::cout << " => SYSTEM : Read file complete." << std::endl;
		} else {
			std::cout << " => ERROR : Cannot open file" << std::endl;
		}
		file_.close();
	}


	/*
	* Function : resetAccount()
	* - Reset value of account's socket to zero when user shutdown or logout
	* Parameter :
	* - SOCKET connSock : number socket of current client
	* - vector<ACCOUNT>& list : list of account
	* Return : void
	*/
	void resetAccount(SOCKET connSock, std::vector<ACCOUNT>& list) {
		for (int i = 0; i < list.size(); i++) {
			if (list[i].socket == connSock) {
				list[i].socket = 0;
				list[i].wrongPassword = 0;
			}
		}
	}


	/*
	* Function : resetAll()
	* - Reset value of all account's socket and wrongPass to zero when server shutdown
	* Parameter :
	* - vector<ACCOUNT>& list : list of account
	* Return : void
	*/
	void resetAll(std::vector<ACCOUNT>& list) {
		for (int i = 0; i < list.size(); i++) {
			list[i].socket = 0;
			list[i].wrongPassword = 0;
		}
	}


	/*
	* Function : updateFile()
	* - Update file if there is account has count of wrong pass word >= 3
	* Parameter :
	* - vector<ACCOUNT>& list : list of account
	* Return : void
	*/
	void updateFile(std::vector<ACCOUNT>& list) {
		std::ofstream file_(FILE_ACCOUNT);

		if (!file_.fail()) {
			for (int i = 0; i < 3; i++) {
				file_ << list[i].id << " " << list[i].pass << " " << list[i].status << std::endl;
			}
		}
		file_.close();
	}


	/*
	* Function : craftMessage()
	* - Craft message to form of struct MESSAGE before sending to client
	* Parameter :
	* - MESSAGE& msg : message that craft to send to server
	* - int status : status code that response to request of user
	* Return : void
	*/
	void craftMessage(MESSAGE& msg, int status) {

		if (status == LOGIN_SUCCESS || status == LOGOUT_SUCCESS) {
			msg.header = OK;
		} else {
			msg.header = ERR;
		}

		sprintf(msg.payload, "%d", status);
		msg.length = strlen(msg.payload);
	}


	/*
	* Function : sendMessage()
	* - Send message to client
	* Parameter :
	* - SOCKET connSock : number socket of current client
	* - MESSAGE& msg : message that send to client
	* - int status : status code that response to request of user
	* Return : int
	* - Value return from process sending
	*/
	int sendMessage(SOCKET connSock, MESSAGE& msg, int status) {

		craftMessage(msg, status);

		cout << " => SYSTEM : Send to client " << connSock << " : ";
		printSendMessage(msg.header, status);
		cout << "--------------------------------------------------\n";

		int ret = send(connSock, (char*)&msg, sizeof(msg), 0);
		if (ret == SOCKET_ERROR) {
			cout << " => ERROR : Cannot send message to client at socket " << connSock << endl;
		}
		return ret;
	}


	/*
	* Function : receiveMessage()
	* - Receive message from client
	* Parameter :
	* - SOCKET connSock : number socket of current client
	* - MESSAGE& msg : message that receive from client
	* Return : int
	* - Value return from process sending
	*/
	int receiveMessage(SOCKET connSock, MESSAGE& msg) {

		int ret;
		ret = recv(connSock, (char*)&msg, sizeof(msg), 0);
		if (ret == SOCKET_ERROR) {
			cout << " => ERROR : Cannot receive message of client at socket " << connSock << endl;
		}
		return ret;
	}
}