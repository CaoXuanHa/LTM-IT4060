#pragma once

#include "C_Resource.h"
#include "C_DataIO.h"

namespace C_Bussiness {
	int checkInput(char* input);
	void login(SOCKET client, char id[], char pass[]);
	void logout(SOCKET client);
	void end(SOCKET client);
	void handlResponse(SOCKET client);
}