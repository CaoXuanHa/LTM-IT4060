#pragma once

#include "C_Resource.h"

namespace C_DataIO {
	char* craftMessage(int type, char*& message);
	int recoverMessage(char* message);
	int sendMessage(SOCKET client, char* message, int type);
	int receiveMessage(SOCKET client, char*& message);
}