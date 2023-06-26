#pragma once

/*------------LIBRARIES-----------------*/
#define _WINSOCK_DEPRECATED_NO_WARNINGS
//#include "stdafx.h"
#include <string.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <fstream>
#include <iostream>
#pragma comment(lib,"Ws2_32.lib")
#pragma warning(disable: 4996)

/*-------------CONSTANT-----------------*/
#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 15199
#define LARGE_BUFF 1024
#define SMALL_BUFF 20
#define FILE_ACCOUNT "account.txt"
#define MAX_USER 3

#define DELIMETER " "
#define END "\n"
#define toString( name ) #name

/*--------------ACCOUNT-----------------*/
typedef struct account {
	int socket;
	char id[25];
	char pass[25];
	int status;
	int wrongPassword;
} ACCOUNT;

/*--------------MESSAGE-----------------*/
typedef struct messsage {
	int header;
	char payload[50];
} MESSAGE;

/*-------------DATA TYPE-----------------*/
enum DATA_TYPE {
	VALID,
	INVALID
};

/*------------MESSAGE TYPE---------------*/
enum MSG_TYPE {
	LENGTH = 100,
	OK = 101,
	ERR = 102,
	LOGIN = 103,
	LOGOUT = 104,
	EXIT = 105
};

/*------------STATUS CODE------------*/
enum STATUS_CODE {
	LOGIN_SUCCESS = 200,
	LOGOUT_SUCCESS = 201,
	WRONG_INPUT = 301,
	ID_NOT_FOUND = 302,
	WRONG_PASSWORD = 303,
	ALREADY_LOGIN = 304,
	HAVENT_LOGIN = 305,
	BLOCKED = 306,
};

