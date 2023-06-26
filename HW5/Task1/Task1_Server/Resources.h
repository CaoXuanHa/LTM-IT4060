#pragma once

/*------------LIBRARIES-----------------*/
#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <string.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <fstream>
#include <iostream>
#include <process.h>
#include <vector>
#pragma comment(lib,"Ws2_32.lib")
#pragma warning(disable: 4996)

/*-------------CONSTANT-----------------*/
#define SERVER_ADDR "127.0.0.1"
#define BUFF_SIZE 1024
#define FILE_ACCOUNT "account.txt"
#define MAX_USER 100
#define MAX_CLIENT 1024
#define DELIMETER " "


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
	int length;
	char payload[64];
} MESSAGE;

/*-------------DATA TYPE-----------------*/
enum DATA_TYPE {
	VALID,
	INVALID
};

/*------------MESSAGE TYPE---------------*/
enum MSG_TYPE {
	OK = 100,
	ERR = 101,
	LOGIN = 102,
	LOGOUT = 103,
	EXIT = 104
};

/*------------STATUS_CODE------------*/
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

