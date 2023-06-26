#pragma once
#include "stdafx.h"
#include "Resources.h"
#include "Bussiness.h"

namespace UI {

	/*
	* Function : menu()
	* - Show menu for user to choose option
	* Parameter :
	* - SOCKET client : number socket of current client
	* Return : void
	*/
	void menu(SOCKET client);


	/*
	* Function : enterIdAndPass()
	* - Let user input their Id, password and check that input is valid or not
	* Parameter :
	* - char id[] : id or username of user
	* - char pass[] : password of user
	* Return : void
	*/
	void enterIdAndPass(char id[], char pass[]);
}