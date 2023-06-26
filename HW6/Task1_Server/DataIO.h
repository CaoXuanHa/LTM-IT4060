#pragma once
#include "stdafx.h"
#include "Resources.h"

namespace DataIO {

	/*
	* Function : printSendMessage()
	* - Print message that send to client to console
	* Parameter :
	* - int type : type of message, OK or ERR
	* - int status : one of value enum STATUS_CODE
	* Return : void
	*/
	void printSendMessage(int type, int status);

	/*
	* Function : loadDataFromText()
	* - Load data from text to list account
	* Parameter :
	* - vector<ACCOUNT>& list : list of account
	* Return : void
	*/
	void loadDataFromText(vector<ACCOUNT>& list);

	/*
	* Function : resetAccount()
	* - Reset value of account's socket to zero when user shutdown or logout
	* Parameter :
	* - SOCKET connSock : number socket of current client 
	* - vector<ACCOUNT>& list : list of account
	* Return : void
	*/
	void resetAccount(SOCKET connSock, vector<ACCOUNT>& list);

	/*
	* Function : resetAll()
	* - Reset value of all account's socket and wrongPass to zero when server shutdown 
	* Parameter :
	* - vector<ACCOUNT>& list : list of account
	* Return : void
	*/
	void resetAll(vector<ACCOUNT>& list);

	/*
	* Function : updateFile()
	* - Update file if there is account has count of wrong pass word >= 3
	* Parameter :
	* - vector<ACCOUNT>& list : list of account
	* Return : void
	*/
	void updateFile(vector<ACCOUNT>& list);

	/*
	* Function : craftMessage()
	* - Craft message to form of struct MESSAGE before sending to client
	* Parameter :
	* - MESSAGE& msg : message that craft to send to server
	* - int status : status code that response to request of user
	* Return : void
	*/
	void craftMessage(MESSAGE& msg, int status);

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
	int sendMessage(SOCKET connSock, MESSAGE& msg, int status);

	/*
	* Function : receiveMessage()
	* - Receive message from client
	* Parameter :
	* - SOCKET connSock : number socket of current client 
	* - MESSAGE& msg : message that receive from client
	* Return : int
	* - Value return from process sending
	*/
	int receiveMessage(SOCKET connSock, MESSAGE& msg);
}

