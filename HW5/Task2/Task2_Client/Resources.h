#pragma once

/*------------[ LIBRARY ]------------*/
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "stdafx.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#pragma comment(lib,"Ws2_32.lib")
#pragma warning(disable:4996)

//Library MD5 to get checksum of file
#include "md5.h"

/*-----------[ CONSTANT ]-------------*/
#define BUFF_SIZE 1024
#define TAIL_FILE ".enc"
#define PAYLOAD_SIZE 64
#define INTEGRITY  0
#define CORRUPTED -1
#define EMPTY -1

using namespace std;

/*---------[ MESSAGE STRUCT ]----------*/
typedef struct Message {
	char opcode;
	unsigned short length;
	char payload[PAYLOAD_SIZE];
};


/*-----------[ OPCODE TYPE ]-----------*/
enum OPCODE {
	ENCRYPT = 1,
	DECRYPT = 2,
	DATA = 3,
	ERR = 4,
	CHECKSUM = 5
};
