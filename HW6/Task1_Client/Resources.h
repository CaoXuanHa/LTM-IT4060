#pragma once

/*--------------LIBRARIES-------------*/
#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#pragma warning(disable: 4996)
#pragma comment(lib,"Ws2_32.lib")

using namespace std;

/*------------CONSTANT---------------*/
#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5500
#define BUFF_SIZE 1024
#define PAYLOAD_SIZE 64
#define DELIMETER " "

/*------------MESSAGE----------------*/
typedef struct messsage {
	int opcode;
	int length;
	char payload[PAYLOAD_SIZE];
} MESSAGE;

/*----------USER'S CHOICE------------*/
enum USER_CHOICE {
	CHOOSE_LOGIN = 1,
	CHOOSE_LOGOUT = 2,
	CHOOSE_EXIT = 3
};

/*-----------INPUT TYPE--------------*/
enum INPUT_TYPE {
	VALID,
	HAS_SPACE,
	TOO_MUCH
};

/*----------MESSAGE TYPE-------------*/
enum MSG_TYPE {
	OK = 100,
	ERR = 101,
	LOGIN = 102,
	LOGOUT = 103,
	EXIT = 104
};

/*------------STATUS CODE------------*/
enum STATUS_CODE {
	LOGIN_SUCCESS = 200,
	LOGOUT_SUCCESS = 201,
	WRONG_INPUT = 301,
	ID_NOT_FOUND = 302,
	WRONG_PASSWORD = 303,
	ALREADY_LOGIN = 304,
	CANT_LOGIN = 305,
	HAVENT_LOGIN = 306,
	BLOCKED = 307,
};