#pragma once
#include "Resources.h"

//FUNTION 1 : CHECK FILE IS EXIST OR NOT
int checkFile(char* filename) {
	FILE* file;
	file = fopen(filename, "r");
	if (file == NULL) {
		return 0;
	} else {
		fclose(file);
		return 1;
	}
}

//FUNTION 2 : GET CHECKSUM OF FILE
char* getChecksumMD5(char* filename) {
	char* checksum = (char*)malloc(sizeof(char) * 33);
	if (checksum != NULL) {
		MD5 md5;
		strncpy(checksum, md5.digestFile(filename), 32);
		return checksum;
	} else {
		return NULL;
	}
}

//FUNTION 3 : COMPARE CHECKSUM TO CHECK INTEGRITY
int compareChecksum(char* fromClient, char* fromServer) {
	for (int i = 0; i < 32; i++) {
		if (fromClient[i] != fromServer[i]) {
			return CORRUPTED;
		}
	}
	return INTEGRITY;
}

//FUNTION 4 : CRAFT MESSAGE TO SEND TO SERVER
void craftMessage(Message& msg, int mode, char* message, int size) {

	switch (mode) {
		case ENCRYPT:
			msg.opcode = '0';
			msg.length = size + 1;
			sprintf(msg.payload, "%d", atoi(message));
			break;

		case DECRYPT:
			msg.opcode = '1';
			msg.length = size + 1;
			sprintf(msg.payload, "%d", atoi(message));
			break;

		case DATA:
			msg.opcode = '2';
			msg.length = size;
			if (msg.length != 0) {
				memcpy(msg.payload, message, size);
			}
			break;
		case ERR:
			msg.opcode = '3';
			msg.length = 0;
			break;
		case CHECKSUM:
			msg.opcode = '4';
			msg.length = size;
			if (msg.length != 0) {
				memcpy(msg.payload, message, 32);
			}
			break;
		default:
			cout << " => ERROR at func 4 : Invalid option." << endl;
			break;
	}
}

//FUNTION 5 : SEND REQUEST THAT CLIENT WANT ENCRYPT/DECRYPT WITH KEY
int sendResquest(SOCKET client, int type, char* filename, int k) {

	Message msg;
	char* key = (char*)malloc(sizeof(char) * 6);
	sprintf(key, "%d", k);

	craftMessage(msg, type, key, strlen(key));
	int ret = send(client, (char*)&msg, sizeof(msg), 0);
	if (ret == SOCKET_ERROR) {
		cout << " => ERROR at func 5 : Cannot send message. Code : %d" << WSAGetLastError() << endl;
		return -1;
	}

	free(key);
	return 0;
}

//FUNTION 6 : SEND FILE TO SERVER TO ENCRYPT/DECRYPT
int sendFile(SOCKET client, char* filename) {

	//READ FILE IN BINARY MODE
	FILE* file;
	file = fopen(filename, "rb");
	if (file == NULL) {
		cout << " => ERROR at func 6 : Failed to open file.\n";
		return -1;
	}

	//GET LENGTH OF FILE
	fseek(file, 0, SEEK_END);
	int nLeft = ftell(file);
	fseek(file, 0, SEEK_SET);

	//START SENDING DATA OF FILE
	int ret;
	Message msg;
	char* buff = (char*)malloc(sizeof(char) * PAYLOAD_SIZE);
	while (nLeft > 0) {

		int size = (nLeft > PAYLOAD_SIZE) ? PAYLOAD_SIZE : nLeft;
		fread(buff, 1, size, file);
		craftMessage(msg, DATA, buff, size);

		ret = send(client, (char*)&msg, sizeof(msg), 0);
		if (ret == SOCKET_ERROR) {
			cout << " => ERROR at func 6 : Cannot send message. Code : %d" << WSAGetLastError() << endl;
			return -1;
		}
		nLeft = nLeft - size;
	}

	//NOTICE SERVER THAT CLIENT COMPLETE SENDING FILE
	craftMessage(msg, DATA, NULL, 0);
	ret = send(client, (char*)&msg, sizeof(msg), 0);
	if (ret == SOCKET_ERROR) {
		cout << " => ERROR at func 6 : Cannot send message. Code : %d" << WSAGetLastError() << endl;
		return -1;
	}
	free(buff);
	fclose(file);

	//SEND CHECKSUM OF FILE
	char* checksum = getChecksumMD5(filename);
	craftMessage(msg, CHECKSUM, checksum, 32);
	ret = send(client, (char*)&msg, sizeof(msg), 0);

	if (ret == SOCKET_ERROR) {
		cout << " => ERROR at func 6 : Cannot send message. Code : %d" << WSAGetLastError() << endl;
		return -1;
	}
	free(checksum);

	//WAITING FOR SERVER RESPONSE THAT FILE IS INTEGRITY OR NOT
	ret = recv(client, (char*)&msg, sizeof(msg), 0);
	if (ret > 0) {
		if (msg.opcode == '4') {
			cout << " => SYSTEM : Complete sending file." << endl;
			return INTEGRITY;
		} else {
			return CORRUPTED;
		}
	} else {
		cout << " => ERROR at func 6 : Cannot receive message. Code : %d" << WSAGetLastError() << endl;
		return -1;
	}

}

//FUNTION 7 : RECEIVE FILE RESULT FROM SERVER
int recvFile(SOCKET client, char* filename, char*& fileResult) {

	//ADD TAIL .enc TO FILE
	FILE* file;
	int length = strlen(filename) + strlen(TAIL_FILE);
	fileResult = (char*)malloc(sizeof(char) * length + 1);

	memset(fileResult, 0, length);
	strncpy(fileResult, filename, strlen(filename));
	strcat(fileResult, TAIL_FILE);

	//WRITE TO FILE RESULT IN BINARY MODE
	file = fopen(fileResult, "wb");
	if (file == NULL) {
		cout << " => ERROR at func 7 : Failed to write file.\n";
		return -1;
	}

	//START RECEIVING DATA OF FILE UNTIL OPCODE = 2, LENGTH = 0
	int ret;
	Message msg = { NULL };
	do {
		ret = recv(client, (char*)&msg, sizeof(msg), 0);
		if (ret > 0) {
			if (msg.opcode == '2') {
				fwrite(msg.payload, 1, msg.length, file);
			}
		} else if (ret == SOCKET_ERROR) {
			cout << " => ERROR at func 7 : Cannot receive message. Code : %d" << WSAGetLastError() << endl;
			return -1;
		}
	} while (msg.opcode == '2' && msg.length != 0);
	fclose(file);

	//WAITING TO RECEIVE MESSAGE HAS OPCODE = 4, MEAN CHECKSUM
	char* checksum = getChecksumMD5(fileResult);
	ret = recv(client, (char*)&msg, sizeof(msg), 0);

	if (ret > 0) {

		//COMPARE CHECKSUM GET FROM SERVER WITH CHECKSUM OF FILE RESULT
		if (compareChecksum(msg.payload, checksum) == INTEGRITY) {
			//FILE IS INTEGRITY
			craftMessage(msg, CHECKSUM, NULL, 0);
			ret = send(client, (char*)&msg, sizeof(msg), 0);
			if (ret == SOCKET_ERROR) {
				cout << " => ERROR at func 7 : Cannot send message. Code : %d" << WSAGetLastError() << endl;
				return -1;
			}
			free(checksum);
			cout << " => SYSTEM : Complete receiving file." << endl;
			return INTEGRITY;

		} else {
			//FILE IS CORRUPTED
			craftMessage(msg, ERR, NULL, 0);
			ret = send(client, (char*)&msg, sizeof(msg), 0);
			if (ret == SOCKET_ERROR) {
				cout << " => ERROR at func 7 : Cannot send message. Code : %d" << WSAGetLastError() << endl;
				return -1;
			}
			free(checksum);
			cout << " => ERROR at func 7 : File is corrupted." << endl;
			return CORRUPTED;
		}
	} else {
		cout << " => ERROR at func 7 : Cannot receive message. Code : %d" << WSAGetLastError() << endl;
		return -1;
	}
}
