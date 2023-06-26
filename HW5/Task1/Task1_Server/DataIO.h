#pragma once
#include "stdafx.h"
#include "Resources.h"

namespace DataIO {

	//Print message reponse to client to sreen
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

	/*-----------------Communicate with file-------------------------*/

	//Get data from text and fill to list user
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

	//Reset only one client
	void resetAccount(SOCKET connSock, std::vector<ACCOUNT>& list) {
		for (int i = 0; i < list.size(); i++) {
			if (list[i].socket == connSock) {
				list[i].socket = 0;
				list[i].wrongPassword = 0;
			}
		}
	}

	//Reset every thing about all client to 0 such as socket number, wrong password number 
	void resetAll(std::vector<ACCOUNT>& list) {
		for (int i = 0; i < list.size(); i++) {
			list[i].socket = 0;
			list[i].wrongPassword = 0;
		}
	}

	//Update status of account to file
	void updateFile(std::vector<ACCOUNT>& list) {
		std::ofstream file_(FILE_ACCOUNT);

		if (!file_.fail()) {
			for (int i = 0; i < 3; i++) {
				file_ << list[i].id << " " << list[i].pass << " " << list[i].status << std::endl;
			}
		}
		file_.close();
	}

	/*-----------------Communicate with client-------------------------*/

	//Add header to messages
	void craftMessage(MESSAGE& msg, int status) {

		if (status == LOGIN_SUCCESS || status == LOGOUT_SUCCESS) {
			msg.header = OK;
		} else {
			msg.header = ERR;
		}

		sprintf(msg.payload, "%d", status);
		msg.length = strlen(msg.payload);

	}

	//Send message to client
	int sendMessage(SOCKET connSock, MESSAGE& msg, int status) {



		craftMessage(msg, status);

		std::cout << " => SYSTEM : Send to client " << connSock << " : ";
		printSendMessage(msg.header, status);
		std::cout << "--------------------------------------------------\n";

		int ret = send(connSock, (char*)&msg, sizeof(msg), 0);
		if (ret == SOCKET_ERROR) {
			return -1;
		}
		return ret;
	}

	//Receive message from client
	int receiveMessage(SOCKET connSock, MESSAGE& msg) {

		int ret = recv(connSock, (char*)&msg, sizeof(msg), 0);
		if (ret == SOCKET_ERROR) {
			return -1;
		}
		return ret;
	}
}

