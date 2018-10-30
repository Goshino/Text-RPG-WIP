#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <fstream>
#include <string>
#include <cmath>
#include <ctime>

using namespace std;

//Public variables for input and game state
bool running = true;
int cmd;

int roomTrack = 0;
static string pName;
int pLevel;
int pExp;
int pExpUp;

	//Public variables for functions
bool characterConfirm = false;


//Prototyping functions, so there won't be chronological conflict
void mobEncounter();
void randomEncounter();
void shopEncounter();
void beginningAdventure();
void beginningMenu();
void bossEncounter();
void dungeonDepth();
void characterInitialize();
void confirmCharacter();


//Main
void main() {
	while (running) {
		beginningMenu();
	}
}



//Definitions
void mobEncounter() {

}

void randomEncounter() {
	srand(time(NULL));
	int numberOfEvents = 4;
	int eventEncounter = rand() % numberOfEvents;
	switch (eventEncounter) {
	case 0:
		shopEncounter();
		break;
	case 1:
		if (roomTrack >= pLevel * 5) {
			bossEncounter();
			break;
		}
	case 2:
		mobEncounter();
		break;
	case 4:
		dungeonDepth();
		roomTrack = 0;
		break;
	}
}

void beginningAdventure() {
	cout << "====================================================\n" << endl << 
		"You stand in front of the entrance to the dungeon..." << endl << 
		"You want to go inside...\n" << endl;
	cout << "[1] Enter Dungeon" << endl <<
		"[0] Exit Game\n" << endl <<
		"====================================================\n" << endl <<
		"Input value of your decision:" << endl;

	cin >> cmd;
	system("cls");

	switch (cmd)
	{
	case 1:
		randomEncounter();
		break;
	case 0:
		running = false;
	default:
		break;
	}
}

void beginningMenu() {
	cout << "===Generic Text Adventure===\n" << endl;
	cout << "[1] Begin Adventure" << endl <<
		"[0] Exit Game\n" << endl <<
		"============================\n" << endl <<
		"Input value of your decision:" << endl;

	cin >> cmd;
	system("cls");

	switch (cmd) {
	case 1:
		characterInitialize();
		beginningAdventure();
		break;
	case 0:
		running = false;
	default:
		break;
	}
}

void bossEncounter() {

}

void dungeonDepth() {

}

void shopEncounter() {

}

void characterInitialize() {
	int characterTypeHP[] = {15, 10, 20};
	int characterTypeAtk[] = {5, 20, 10};
	cout << "=============================\n" << endl <<
		"What is your name adventurer?\n " << endl <<
		"=============================\n" << endl <<
		"Your name:" << endl;

	cin >> pName;
	system("cls");
	do {
		cout << "==============\n" << endl <<
			"And you are...\n" << endl <<
			"[1] a human..." << endl <<
			"[2] an elf..." << endl <<
			"[3] an orc...\n" << endl <<
			"==============\n" << endl;
		cout << "Input value of your decision:" << endl;

		cin >> cmd;
		system("cls");

		switch (cmd)
		{
		case 1:
			confirmCharacter();
			break;
		case 2:
			confirmCharacter();
			break;
		case 3:
			confirmCharacter();
			break;
		}
	} while (!confirmCharacter);
}

void confirmCharacter() {
	char ch; 
	cout << "===================\n" << endl <<
		"Are you sure? (Y/N)\n" << endl <<
		"===================\n" << endl <<
		"Input value of your decision:" << endl;
	cin >> ch;
	ch == 'y' && ch == 'Y' ? characterConfirm = true : characterConfirm = false;

	system("cls");
}
