#include "stdafx.h"
#include "UI.h"

namespace UI {

	/*
	* Function : menu()
	* - Show menu for user to choose option
	* Parameter :
	* - SOCKET client : number socket of current client 
	* Return : void
	*/
	void menu(SOCKET client) {
		char id[BUFF_SIZE];
		char pass[BUFF_SIZE];

		int choice;
		int count = 0;
		char* input = (char*)malloc(sizeof(char) * BUFF_SIZE);

		do {
			system("cls");
			cout << "\n**************************************";
			cout << "\n*    Programmed by Duong Xuan Hoan    *";
			cout << "\n*                                     *";
			cout << "\n*------------[ WELCOME ]--------------*";
			cout << "\n*                                     *";
			cout << "\n*    1. Login                         *";
			cout << "\n*    2. Logout                        *";
			cout << "\n*    3. Exit                          *";
			cout << "\n*                                     *";
			cout << "\n*-------------------------------------*\n";
			do {
				cout << "Please enter your selection : ";
				cin.getline(input, BUFF_SIZE);
				if (strlen(input) > 1 || atoi(input) == 0) {
					choice = 0;
				} else {
					choice = atoi(input);
				}

				switch (choice) {
					case CHOOSE_LOGIN:
						enterIdAndPass(id, pass);
						Bussiness::login(client, id, pass);
						Bussiness::handlResponse(client);
						break;
					case CHOOSE_LOGOUT:
						Bussiness::logout(client);
						Bussiness::handlResponse(client);
						break;
					case CHOOSE_EXIT:
						cout << "Goodbye." << endl;
						break;
					default:
						cout << "WARNING : Wrong input. ";
						break;
				}
				cout << "\n---------------------------------------" << endl;
				count++;
			} while (choice != CHOOSE_EXIT && count != 5);
			count = 0;
			system("pause");
		} while (choice != CHOOSE_EXIT);

		return;
	}

	
	/*
	* Function : enterIdAndPass()
	* - Let user input their Id, password and check that input is valid or not
	* Parameter :
	* - char id[] : id or username of user
	* - char pass[] : password of user
	* Return : void
	*/
	void enterIdAndPass(char id[], char pass[]) {
		int status = 0;
		do {
			switch (status) {
				case VALID:
					break;
				case HAS_SPACE:
					cout << "WARNING : Username or Password doesn't allow to have space" << endl;
					break;
				case TOO_MUCH:
					cout << "WARNING : Username or Password only has 20 character" << endl;
					break;
			}
			cout << "Username : ";
			cin.getline(id, BUFF_SIZE);

			cout << "Password : ";
			cin.getline(pass, BUFF_SIZE);

			status = max(Bussiness::checkInput(id), Bussiness::checkInput(pass));

		} while (status != VALID);
	}
}