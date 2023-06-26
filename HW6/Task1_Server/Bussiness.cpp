#include "stdafx.h"
#include "Bussiness.h"

namespace Bussiness {

	/*
	* Function : checkData()
	* - Check data in message from client is valid or not
	* Parameter :
	* - char* data : data that want to check is valid or not
	* Return : int
	* - Value of enum Valid or Invalid
	*/
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


	/*
	* Function : checkAccount()
	* - Check account status get from client
	* Parameter :
	* - SOCKET connSock : number socket of current client 
	* - vector<ACCOUNT>& list : list of account
	* - ACCOUNT& acc : account that has id and pass receive from user
	* - int type : type of request, LOGIN or LOGOUT
	* Return : int
	* - Value of enum STATUS_CODE
	*/
	int checkAccount(SOCKET connSock, vector<ACCOUNT>& list, ACCOUNT& acc, int type) {

		//Check account status when client want to log in
		if (type == LOGIN) {
			for (int i = 0; i < list.size(); i++) {
				if (list[i].socket == connSock) {
					return ALREADY_LOGIN;
				}
			}
			for (int i = 0; i < list.size(); i++) {

				if (strcmp(list[i].id, acc.id) == 0) {
					if (strcmp(list[i].pass, acc.pass) == 0) {
						if (list[i].status == 0) {
							if (list[i].socket == 0) {
								list[i].socket = connSock;
								acc.socket = connSock;

								return LOGIN_SUCCESS;

							} else {

								return CANT_LOGIN;
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

		//Check account status when client want to log out
		} else if (type == LOGOUT) {
			for (int i = 0; i < list.size(); i++) {
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

	/*
	* Function : getIdAndPass()
	* - Get id and pass from user
	* Parameter :
	* - MESSAGE msg : message receive from user
	* Return : ACCOUNT
	* - current account
	*/
	ACCOUNT getIdAndPass(MESSAGE msg) {
		ACCOUNT acc = { NULL };

		//Copy msg.payload to buff to get id and pass
		char* buff = (char*)malloc(sizeof(char) * msg.length);
		strncpy(buff, msg.payload, msg.length);
		buff[msg.length - 1] = 0;

		char* id = (char*)malloc(sizeof(char) * 20);
		char* pass = (char*)malloc(sizeof(char) * 20);

		memset(id, '\0', 20);
		memset(pass, '\0', 20);

		//Split buff to id and pass
		id = strtok_s(buff, DELIMETER, &buff);
		strcpy(pass, buff);

		//Check id and pass is valid or not
		if (checkData(id) == VALID && checkData(pass) == VALID) {
			strncpy(acc.id, id, strlen(id));
			strncpy(acc.pass, pass, strlen(pass));
			return acc;
		} else {
			return { NULL };
		}
	}
}