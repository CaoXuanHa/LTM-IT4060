#pragma once

#include "stdafx.h"
#include "Resources.h"
#include "DataIO.h"

namespace Bussiness {

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
	int checkAccount(SOCKET connSock, std::vector<ACCOUNT>& list, ACCOUNT& acc, int type) {

		HANDLE mutex;
		mutex = CreateMutex(NULL, FALSE, NULL);

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

							//Lock data to let only one thread can write at one time
							WaitForSingleObject(mutex, INFINITE);
							if (list[i].socket == 0) {
								list[i].socket = connSock;
								acc.socket = connSock;

								//Release lock when thread done written
								ReleaseMutex(mutex);
								CloseHandle(mutex);
								return LOGIN_SUCCESS;

							} else {
								//Release lock when thread done written
								ReleaseMutex(mutex);
								CloseHandle(mutex);
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

	//Get id and pass from message LOGIN
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
		free(buff);
		free(id);
		free(pass);
	}
}


