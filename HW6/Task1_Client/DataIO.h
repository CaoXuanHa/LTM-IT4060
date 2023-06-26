#pragma once

#include "stdafx.h"
#include "Resources.h"

namespace DataIO {

	/*
	* Function : craftMessage()
	* - Craft message to form of struct MESSAGE before sending to server
	* Parameter :
	* - MESSAGE& msg : message that craft to send to server
	* - int type : type of message that want to send to server
	* - char* message : message that want to send to server
	* - int length : length of message
	* Return : void
	*/
	void craftMessage(MESSAGE& msg, int type, char* message, int length);

	/*
	* Function : sendMessage()
	* - Send message to server
	* Parameter :
	* - SOCKET client : number socket of current client
	* - char* message : message that want to send to server
	* - int type : type of message that want to send to server
	* Return : int
	* - Value return from process sending
	*/
	int sendMessage(SOCKET client, char* message, int type);


	/*
	* Function : recvMessage()
	* - Send message to server
	* Parameter :
	* - SOCKET client : number socket of current client
	* - MESSAGE& msg : message that receive from server
	* Return : int
	* - Value return from process receiving
	*/
	int recvMessage(SOCKET client, MESSAGE& msg);
}