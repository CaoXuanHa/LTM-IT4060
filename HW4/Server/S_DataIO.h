#pragma once
#include "S_Resource.h"

namespace S_DataIO {

	//Add header to message
	char* craftMessage(int status);

	/*-----------------Communicate with file-------------------------*/

		//Get data from text and fill to list user
	void loadDataFromText(ACCOUNT list[]);

	//Reset only one client
	void resetAccount(SOCKET connSock, ACCOUNT list[]);

	//Reset every thing about all client to 0 such as socket number, wrong password number 
	void resetAll(ACCOUNT list[]);

	//Update status of account to file
	void updateFile(ACCOUNT list[]);

	/*-----------------Communicate with client-------------------------*/

		//Send message to client
	int receiveMessage(SOCKET connSock, char*& message);

	//Receive message from client
	int getLength(char*& buff);

	//Get length of message
	int sendMessage(SOCKET connSock, char* message);

	//Print message reponse to client to sreen
	void printSendMessage(int type, int status);
}