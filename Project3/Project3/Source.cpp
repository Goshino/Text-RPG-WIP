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
int roomTrack = 0;
int pLevel;
int cmd; 


//Prototyping functions, so there won't be chronological conflict
void mobEncounter();
void randomEncounter();
void shopEncounter();
void beginningAdventure();
void beginningMenu();
void bossEncounter();
void dungeonDepth();



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
	cout << "You stand in front of the entrance to the dungeon, you want to go inside.\n" << endl;
	cout << "[1] Enter Dungeon" << endl <<
		"[0] Exit Game\n" << endl <<
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