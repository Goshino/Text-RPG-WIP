#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <cmath>
#include <ctime>

using namespace std;

struct Enemy {
	string e_name; 

	int e_level;
	int e_hpMax;
	int e_hp;

	int e_atk;
	int e_favoriteAtk; //0 for block, 1 for normal, 2 for heavy
	double e_atkSpd; //0.5 for slow, 1 for normal, 2 for fast
	int e_armr;

	double e_critChance;
	double e_dodgeChance;
	int e_expDrop;
};

//Public variables for input and game state
bool running = true; //Game state
bool runChoice = true;
bool isAlive = true;
int cmd; //User input

//Reason for this is so that the player does not automatically encounter a boss, scales with player level
static string pName; //Player name
static string species; //Player species, option given in the game
int p_level = 1; //Player level
int p_exp = 0; //Player current exp
int p_expUp = 0; //Exp that is required to level up to the next level, assuming player has 0 exp

int toughness = 0; //Skillpoint based around armor
int vitality = 0; //Skillpoint based around health
int strength = 0; //Skillpoint based around attack
int dexterity = 0; //Skillpoint based around Crit chance
int agility = 0; //Skillpoint based around dodge chance

int p_hpMax = 0; //Player max hp
int p_hp = 0; //Player hp, this is different from max exp as the player is expected to get damaged
int p_atk = 0; //Player damage
int p_atkSpd = 0;
double p_critChance = 0; //Chance for the player to hit a crit; doubles the amount of damage dealt to the mob
double p_dodgeChance = 0; //Cahnce for the player to dodge an attack; enemy attack is completely nullified
int p_armr = 0; //Player's armor stat, used to dedtermine the player's damage resistance

//Public variables for functions
bool characterConfirm = false;
int roomTrack = 0; //Tracks the amount of rooms that the player has entered with each dungeon floor


//Prototyping functions, so there won't be chronological conflict
Enemy enemyCreate(); //Enemy struct that randomly generates an enemy
void combat();
void randomEncounter(); //Function that forces a random encounter whenever the player enters a room: shopkeeper, rest, mob, boss, dungeon level
void shopEncounter(); //Generates a shop for the player
void bossEncounter(); //Spawns boss
void dungeonProgress(); //Function where the player progresses further into the dungeon level wise

void beginningAdventure(); //Function that actually begins the adventure
void beginningMenu(); //UI for beginning menu, prompts the rest of the game

void characterInitialize(); //Function that initializes player stat variables for the start of the game
void confirmCharacter(int n, string arr[], int arr2[], double arr3[]); //Function used in characterInitialize, used to confirm the character stats

void printEnemyStats(); //Randomly generates enemy stats
void printPlayerStats(); //Function that shows the player his or her stats 

void levelUp(int& level); //Function that updates the player's stats updated after a completion of every room
void specUp(); //Function that allows the user where he or she wants to put his or her skill 

int rnd(double n); //Function for rounding
int expUpAlg(int level); //Function that decides the amount needed to level up the player
int rand_int(int low, int high); //Function that easily translates the rand % x algorithm into a function, not necessary but nice to have

void tryAgain();

struct Inventory {
	int min;
	int max;
	int capacity;
};

//Main
void main() {
	do {
		beginningMenu();
		if (!isAlive) {
			tryAgain();
		}
	} while (running && runChoice);

	/*
	for (size_t i = 0; i < 100; i++) {
		Enemy temp = enemyCreate();
		cout << temp.e_name << endl;
		cout << temp.e_hp << '/' << temp.e_hpMax << endl;
		cout << temp.e_atk << endl;
		cout << endl;
	}
	*/
}



//Definitions
Enemy enemyCreate() {
	int type = rand_int(0, 2);
	Enemy temp;
	temp.e_level = p_level;
	switch (type) {
	case 0: //Goblins
		temp.e_name = "Goblin";
		temp.e_hp = temp.e_hpMax = rand_int(5 * temp.e_level, 10 * temp.e_level);
		temp.e_atk = rand_int(1 * temp.e_level, 5 * temp.e_level);
		temp.e_favoriteAtk = 1; //Normal attack
		temp.e_atkSpd = 2; //Fast attack speed
		break;
	case 1: //Golems
		temp.e_name = "Golem";
		temp.e_hp = temp.e_hpMax = rand_int(15 * temp.e_level, 20 * temp.e_level);
		temp.e_atk = rand_int(10 * temp.e_level, 15 * temp.e_level);
		temp.e_favoriteAtk = 2;
		temp.e_atkSpd = 0.5;
		break;
	case 2: //Slimes
		temp.e_name = "Slimes";
		temp.e_hp = temp.e_hpMax = rand_int(10 * temp.e_level, 15 * temp.e_level);
		temp.e_atk = rand_int(5 * temp.e_level, 10 * temp.e_level);
		temp.e_favoriteAtk = 0;
		temp.e_atkSpd = 1;
		break;
	}
	return temp; 
}

void randomEncounter() { //This function creates an encounter randomly for the game
	srand(time(NULL));
	int numberOfEvents = 4; //The amount of significant events in the game; takes a similarity to parallelism
	int eventEncounter = rand() % numberOfEvents; //Used in the switch statement to decide which, creates a random value from 0-3
	switch (eventEncounter) {
	case 0: //Player encounters a shop
		shopEncounter();
		break;
	case 1: //Player encounters a boss
		if (roomTrack >= rnd(p_level * 5 / 2)) {
			bossEncounter();
			break;
		}
	case 2: //Player encounters a mob, handled by the function mobEncounter
		combat();
		break;
	case 3: //Player goes deeper into the dungeon, floor level wise
		dungeonProgress();
		roomTrack = 0;
		break;
	}
}

void beginningAdventure() { //Function that prompts after the beginningMenu function. This transitions into 2 other functions
	cout << "====================================================\n" << endl <<
		"You stand in front of the entrance to the dungeon..." << endl <<
		"You want to go inside...\n" << endl;
	cout << "[1] Enter Dungeon" << endl <<
		"[2] View Stats" << endl <<
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
	case 2: //printPlayerStats function is called, after that, beginningAdventure function is called once more for the player to decide what to do
		printPlayerStats();
		system("pause");
		system("cls");
		beginningAdventure();
		break;
	case 0:
		running = false;
	default:
		break;
	}
}

void beginningMenu() { //beginingMenu function allows the user to begin his or her adventure, prompts character initialization after name declaration
	cout << "===Generic Text Adventure===\n" << endl;
	cout << "[1] Begin Adventure" << endl <<
		"[0] Exit Game\n" << endl <<
		"============================\n" << endl <<
		"Input value of your decision:" << endl;

	cin >> cmd;
	system("cls");

	switch (cmd) {
	case 1:
		cout << "=============================\n" << endl <<
			"What is your name adventurer?\n " << endl <<
			"=============================\n" << endl <<
			"Your name:" << endl;
		cin >> pName;
		characterInitialize();
		beginningAdventure();
		break;
	case 0:
		running = false;
	default:
		break;
	}
}

void combat() {
	Enemy temp = enemyCreate();
	cout << "You have encountered an enemy!" << endl;

	system("pause");
	system("cls");

	cout << "Combat" << endl;
}

void bossEncounter() {

}

void dungeonProgress() {
	cout << "===========Dungeon Progress===========\n" << endl <<
		"You descend deeper into the dungeon..." << endl <<
		"[1] Free Level Up" << endl <<
		"[2] Free Loot\n" << endl <<
		"======================================" << endl;

	cin >> cmd;
	system("cls");

	switch (cmd) {
	case 1:
		p_exp += expUpAlg(p_level) - p_exp;
		break;
	case 2:
		break;
	}
}

void shopEncounter() {

}

void characterInitialize() {
	string characterSpecies[3] = { "Human", "Elf", "Orc" };
	int characterTypeHP[3] = { 15, 10, 20 }; //Array for three archtypes of characters
	double characterAtkSpd[3] = { 1, 2, 0.5 }; //Array for the three archtypes' attack speed

	system("cls");
	do {
		cout << "============================\n" << endl <<
			pName << "... you are...\n" << endl <<
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
			cout << "==========================================================\n" << endl;
			cout << "Humans have a base health of " << characterTypeHP[cmd - 1] << "..." << endl;
			cout << "They use normal attacks and have an attack at a speed of " << 1 << endl << endl;
			cout << "==========================================================\n" << endl;
			system("pause");
			system("cls");
			confirmCharacter(cmd - 1, characterSpecies, characterTypeHP, characterAtkSpd);
			break;
		case 2: //Elf player
			cout << "================================================\n" << endl;
			cout << "Elves have a base health of " << characterTypeHP[cmd - 1] << "..." << endl;
			cout << "Dancing with combat, they attack at a speed of " << 2 << endl << endl;
			cout << "================================================\n" << endl;
			system("pause");
			system("cls");
			confirmCharacter(cmd - 1, characterSpecies, characterTypeHP, characterAtkSpd);
			break;
		case 3: //Orc player
			cout << "=================================================\n" << endl;
			cout << "Orcs have a base health of " << characterTypeHP[cmd - 1] << "..." << endl;
			cout << "Using heavy attacks they attack at a speed of " << 0.5 << endl << endl;
			cout << "=================================================\n" << endl;
			system("pause");
			system("cls");
			confirmCharacter(cmd - 1, characterSpecies, characterTypeHP, characterAtkSpd);
			break;
		}
	} while (!characterConfirm);
}


void confirmCharacter(int n, string arr[], int arr2[], double arr3[]) {
	char ch;
	do {
		cout << "=============================\n" << endl <<
			"Are you sure? (Y/N)\n" << endl <<
			"=============================\n" << endl <<
			"Input value of your decision:" << endl;
		cin >> ch;
		ch == 'y' || ch == 'Y' ? characterConfirm = true : characterConfirm = false;
		if (!characterConfirm) {
			characterInitialize();
		}
		else {
			species = arr[n];
			p_hpMax = p_hp = arr2[n];
			p_atk = 5;
			p_atkSpd = arr3[n];
		}
	} while (!characterConfirm);

	system("cls");

}

void printPlayerStats() { //Function that prints all of the character information, used when player wants to view stats
	cout << "=================Player Stats=================\n" << endl;
	cout << "Name: " << pName << endl;
	cout << "Species: " << species << endl;
	cout << "Level: " << p_level << endl;
	cout << "Exp to level up: " << expUpAlg(p_level) << endl << endl;
	cout << "HP: " << p_hp << '/' << p_hpMax << endl;
	cout << "Attack: " << p_atk << endl;
	cout << "Attack Speed: " << p_atkSpd << endl;
	cout << "Armor: " << p_armr << endl << endl;
	cout << "Crit Chance: " << p_critChance << '%' << endl;
	cout << "Dodge Chance: " << p_dodgeChance << '%' << endl << endl;
	cout << "Toughness (Impacts Armor): " << toughness << endl;
	cout << "Vitality (Impacts Health): " << vitality << endl;
	cout << "Strength (Impacts Strength): " << strength << endl;
	cout << "Dexterity (Impacts Critical Chance): " << dexterity << endl;
	cout << "Agility (Impacts Dodge Chance): " << agility << endl << endl;
	cout << "==============================================" << endl;
}

void printEnemyStats() { //Enemy stats. This function intializes enemy mob stats based on their archtype as well as scaling with the player's current level
	cout << "Level:" << p_level << endl <<
		"HP:";
}

void levelUp(int& level) { //Function that updates the player stats, called whenever the player wants to view stats or finishes a room
	while (p_exp > p_expUp) {
		level++;
		p_exp -= expUpAlg(level);
		p_expUp = expUpAlg(level);
		specUp();
	}
}

void specUp() {
	cout << "=====================================\n" <<
		"Decide where to put your skill points\n" << endl <<
		"[1] Toughness (Impacts Armor)" <<
		"[2] Vitality (Impacts Health)" << endl <<
		"[3] Strength (Impacts Attack)" << endl <<
		"[4] Dexterity (Impacts Crit Chance)" << endl <<
		"[5] Agility (Impacts Dodge Chance)\n" << endl <<
		"Input value of your decision: " << endl;

	cin >> cmd;
	system("cls");

	switch (cmd)
	{
	case 1:
		toughness++;
		break;
	case 2:
		vitality++;
		break;
	case 3:
		strength++;
		break;
	case 4:
		dexterity++;
		break;
	case 5:
		agility++;
		break;
	}
}

void tryAgain() {
	char ch;
	cout << "==========Game Over==========\n" << endl <<
		"Would you like to try again? (Y/N)\n" << endl <<
		"=============================\n" << endl <<
		"Input value of your decision: " << endl;

	cin >> ch;
	system("cls");

	((ch == 'Y') || (ch == 'y')) ? runChoice = true : runChoice = false;
}

int rnd(double n) {  //Simple round function with integer double addition
	return n + 0.5;
}

int expUpAlg(int level) { //Function definition of expUpAlg,used to compute how much exp is required to hit the next level assuming exp isat 0
	return rnd(4 * pow(level, 3) / 5);
}

int rand_int(int low, int high) {
	int diff = high - low + 1;
	return rand() % diff + low;
}
