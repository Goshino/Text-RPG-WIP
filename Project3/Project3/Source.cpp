#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <cmath>
#include <ctime>

using namespace std;

//Public variables for input and game state
bool running = true; //Game state
int cmd; //User input

int roomTrack = 0; //Tracks the amount of rooms that the player has entered with each dungeon floor
				   //Reason for this is so that the player does not automatically encounter a boss, scales with player level
static string pName; //Player name
int pLevel; //Player level
int pExp; //Player current exp
int pExpUp; //Exp that is required to level up to the next level, assuming player has 0 exp

int toughness; //Skillpoint based around armor
int vitality; //Skillpoint based around health
int strength; //Skillpoint based around attack
int dexterity; //Skillpoint based around Crit chance
int agility; //Skillpoint based around dodge chance

int pHp; //Player hp, this is different from max exp as the player is expected to get damaged
int pHpMax; //Player max hp
int pAtk; //Player damage
double pCritChance; //Chance for the player to hit a crit; doubles the amount of damage dealt to the mob
double pDodgeChance; //Cahnce for the player to dodge an attack; enemy attack is completely nullified
int pArmr; //Player's armor stat, used to dedtermine the player's damage resistance
double pDmgRes; //Damage resistance, the amount of damage that is blocked from an attack, x% effective due to the player's armor

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
void enemyStats();
void playerStats();

//Main
void main() {
	while (running) {
		beginningMenu();
	}
}



//Definitions
void mobEncounter() {
	srand(time(NULL));
	int typesOfMobs = 3;
	int encounterMob = rand() % typesOfMobs;
	switch (encounterMob)
	{
	case 0: //Encounters a mob that specializes in normal attacks
		break;
	case 1: //Encounters a mob that specializes in heavy attacks
		break;
	case 2: //Encounters a mob that specializes in blocking or avoiding damage
		break;
	}
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
	int characterTypeHP[] = { 15, 10, 20 };
	int characterTypeAtk[] = { 5, 20, 10 };
	cout << "=============================\n" << endl <<
		"What is your name adventurer?\n " << endl <<
		"=============================\n" << endl <<
		"Your name:" << endl;

	cin >> pName;
	system("cls");
	do {
		cout << "============================\n" << endl <<
			pName <<"... you are...\n" << endl <<
			"[1] a human..." << endl <<
			"[2] an elf..." << endl <<
			"[3] an orc...\n" << endl <<
			"============================\n" << endl;
		cout << "Input value of your decision:" << endl;

		cin >> cmd;
		system("cls");

		switch (cmd)
		{
		case 1: //Human player
			cout << "===============================\n" << endl;
			cout << "Humans have a base health of 15\n" << endl;
			cout << "===============================\n" << endl;
			system("pause");
			system("cls");
			confirmCharacter();
			break;
		case 2: //Elf player
			cout << "==============================\n" << endl;
			cout << "Elves have a base health of 10\n" << endl;
			cout << "==============================\n" << endl;
			system("pause");
			system("cls");
			confirmCharacter();
			break;
		case 3: //Orc player
			cout << "=============================\n" << endl;
			cout << "Orcs have a base health of 20\n" << endl;
			cout << "=============================\n" << endl;
			system("pause");
			system("cls");
			confirmCharacter();
			break;
		}
	} while (!characterConfirm);
}


void confirmCharacter() {
	char ch;
	cout << "===================\n" << endl <<
		"Are you sure? (Y/N)\n" << endl <<
		"===================\n" << endl <<
		"Input value of your decision:" << endl;
	cin >> ch;
	ch == 'y' || ch == 'Y' ? characterConfirm = true : characterConfirm = false;

	system("cls");


}

void playerStats() {
	cout << "Character Sheet" << endl;
	cout << "Name: " << pName << endl;
	cout << "Level: " << pExp << endl;
	cout << "Exp to level up: " << pExpUp - pExp << endl << endl;
	cout << "HP: " << pHp << '/' << pHpMax << endl;
	cout << "Attack: " << pAtk << endl; 
	cout << "Armor: " << pArmr << endl;
	cout << "Damage Res: " << pDmgRes << '%' << endl;
}

void enemyStats() {

}
