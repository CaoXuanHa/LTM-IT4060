#pragma once

#include "stdafx.h"
#include "Resources.h"
#include "DataIO.h"

namespace Bussiness {

	/*
	* Function : checkData()
	* - Check data in message from client is valid or not
	* Parameter :
	* - char* data : data that want to check is valid or not
	* Return : int
	* - Value of enum Valid or Invalid
	*/
	int checkData(char* data);


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
	int checkAccount(SOCKET connSock, vector<ACCOUNT>& list, ACCOUNT& acc, int type);
	

	/*
	* Function : getIdAndPass()
	* - Get id and pass from user
	* Parameter :
	* - MESSAGE msg : message receive from user
	* Return : ACCOUNT
	* - current account
	*/
	ACCOUNT getIdAndPass(MESSAGE msg);
}


