#pragma once

/*--------------LIBRARIES-------------*/
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#pragma warning(disable: 4996)
#pragma comment(lib,"Ws2_32.lib")

/*------------CONSTANT---------------*/
#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 15199
#define SMALL_BUFF 1024
#define LARGE_BUFF 2048

#define DELIMETER " "
#define END "\n"


/*----------USER'S CHOICE------------*/
enum USER_CHOICE {
	CHOOSE_LOGIN  = 1,
	CHOOSE_LOGOUT = 2,
	CHOOSE_EXIT   = 3
};

/*-----------INPUT TYPE--------------*/
enum INPUT_TYPE {
	VALID,
	HAS_SPACE,
	TOO_MUCH
};

/*----------MESSAGE TYPE-------------*/
enum MSG_TYPE {
	LENGTH = 100,
	OK     = 101,
	ERR    = 102,
	LOGIN  = 103,
	LOGOUT = 104,
	EXIT   = 105
};

/*------------STATUS CODE------------*/
enum STATUS_CODE {
	LOGIN_SUCCESS  = 200,
	LOGOUT_SUCCESS = 201,
	WRONG_INPUT    = 301,
	ID_NOT_FOUND   = 302,
	WRONG_PASSWORD = 303,
	ALREADY_LOGIN  = 304,
	HAVENT_LOGIN   = 305,
	BLOCKED        = 306,
};