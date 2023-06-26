#pragma once
#include "stdafx.h"
#include "Resources.h"
#include "DataIO.h"

namespace Bussiness {

	/*
	* Function : checkInput()
	* - Check input that is too much character or has space or valid
	* Parameter :
	* - char* input : input get from user
	* Return : int
	* - Value of enum INPUT_TYPE
	*/
	int checkInput(char* input);

	/*
	* Function : login()
	* - User send login request to server
	* Parameter :
	* - SOCKET client : number socket of current client
	* - char id[] : id or username of user
	* - char pass[] : password of user
	* Return : void
	*/
	void login(SOCKET client, char id[], char pass[]);

	/*
	* Function : logout()
	* - User send logout request to server
	* Parameter :
	* - SOCKET client : number socket of current client
	* Return : void
	*/
	void logout(SOCKET client);

	/*
	* Function : handlResponse()
	* - Get message from server and print result to console
	* Parameter :
	* - SOCKET client : number socket of current client
	* Return : void
	*/
	void handlResponse(SOCKET client);
}

