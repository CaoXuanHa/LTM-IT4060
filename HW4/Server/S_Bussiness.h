#pragma once
#include "S_Resource.h"
#include "S_DataIO.h"

namespace S_Bussiness {
	//Check data in message from client is valid or not
	int checkAccount(SOCKET connSock, ACCOUNT list[], ACCOUNT& acc, int type);


	//Check account status get from client
	int handleMessage(MESSAGE& msg, char*& buff);

	//Get id and pass from message LOGIN
	int checkData(char* data);

	//Handle message got from client 
	ACCOUNT getIdAndPass(char* payload);
}