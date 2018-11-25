#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <cmath>
#include <ctime>

using namespace std;

enum Species {
	Human, Elf, Orc 
};

struct Enemy { //Stats are initialized to zero before run time, this will be changed at run time
	string e_species;
	bool e_isAlive;

	int e_level = 0;
	int e_hpMax = 0;
	int e_hp = 0;

	int e_atk = 0;
	int e_favoriteAtk = 0; //0 for counter, 1 for normal, 2 for heavy
	int e_atkChoice = 0; // 0 for counter, 1 for normal, 2 for heavy
	double e_atkSpd = 0; //0.5 for slow, 1 for normal, 2 for fast
	int e_armr = 0;

	double e_critChance = 0;
	double e_dodgeChance = 0;
	int e_expDrop = 0;
};

//Public variables for input and game state
bool running = true; //Game state
bool runChoice = true;
bool p_isAlive = true;
int cmd; //User input

//Reason for this is so that the player does not automatically encounter a boss, scales with player level
static string p_name; //Player name
Species p_species; //Player species, option given in the game
int p_level = 1; //Player level
int p_exp = 0; //Player current exp
int p_expUp = 0; //Exp that is required to level up to the next level, assuming player has 0 exp
int p_gold = 0;

int toughness = 0; //Skillpoint based around armor
int vitality = 0; //Skillpoint based around health
int strength = 0; //Skillpoint based around attack
int dexterity = 0; //Skillpoint based around Crit chance
int agility = 0; //Skillpoint based around dodge chance

int p_hpMax = 0; //Player max hp
int p_hp = 0; //Player hp, this is different from max hp as the player is expected to get damaged
int p_atk = 0; //Player damage
int p_atkChoice = 0;
int p_atkSpd = 0;
double p_critChance = 0; //Chance for the player to hit a crit; doubles the amount of damage dealt to the mob
double p_dodgeChance = 0; //Cahnce for the player to dodge an attack; enemy attack is completely nullified
int p_armr = 0; //Player's armor stat, used to dedtermine the player's damage resistance

//Public variables for functions
bool killedBoss = false;
bool characterConfirm = false;
int roomTrack = 0; //Tracks the amount of rooms that the player has entered with each dungeon floor


//Prototyping functions, so there won't be chronological conflict
Enemy enemyCreate(); //Enemy struct that randomly generates an enemy

void combat(Enemy mob);
void attack(Enemy mob, int p_atkChoice);
bool checkState(Enemy mob);
void enemyDiceRoll(Enemy& mob);
bool doesPlayerWin(Enemy mob, int p_atkChoice);
void combatNotif();
void endOfCombat(Enemy mob);

void randomEncounter(); //Function that forces a random encounter whenever the player enters a room: shopkeeper, rest, mob, boss, dungeon level
void shopEncounter(); //Generates a shop for the player
void bossEncounter(); //Spawns boss
void dungeonProgress(); //Function where the player progresses further into the dungeon level wise

void beginningAdventure(); //Function that actually begins the adventure
void beginningMenu(); //UI for beginning menu, prompts the rest of the game

void characterInitialize(); //Function that initializes player stat variables for the start of the game
void confirmCharacter(int n, int arr2[], double arr3[]); //Function used in characterInitialize, used to confirm the character stats

void printEnemyStats(Enemy mob);
void printPlayerStats(); //Function that shows the player his or her stats 

void levelUp(int& level); //Function that updates the player's stats updated after a completion of every room
void specUp(); //Function that allows the user where he or she wants to put his or her skill 
void statUpdate();

int rnd(double n); //Function for rounding
int expUpAlg(int level); //Function that decides the amount needed to level up the player
int rand_int(int low, int high); //Function that easily translates the rand % x algorithm into a function, not necessary but nice to have

void tryAgain();

//Main
void main() {

	do {
		beginningMenu();
		if (!p_isAlive) {
			tryAgain();
		}
	} while (running && runChoice);

	system("pause");
}

//Definitions
Enemy enemyCreate() {
	srand(time(NULL));
	int type = rand_int(0, 2);
	Enemy temp;
	temp.e_level = rand_int(p_level, p_level + 5);
	switch (type) {
	case 0: //Goblins
		temp.e_species = "Goblin";
		temp.e_hp = temp.e_hpMax = rnd(sqrt(temp.e_level) * 10 + (temp.e_level / 3));
		temp.e_atk = rnd(sqrt(temp.e_level * 10) * (3 / 2));
		temp.e_favoriteAtk = 1; //Normal attack
		temp.e_atkSpd = 2; //Fast attack speed
		break;
	case 1: //Golems
		temp.e_species = "Golem";
		temp.e_hp = temp.e_hpMax = rnd(sqrt(temp.e_level) * 10) + temp.e_level;
		temp.e_atk = rnd(sqrt(temp.e_level * 10) * 3);
		temp.e_favoriteAtk = 2; //Heavy Attack
		temp.e_atkSpd = 0.5;
		break;
	case 2: //Slimes
		temp.e_species = "Slimes";
		temp.e_hp = temp.e_hpMax = rnd(sqrt(temp.e_level) * 10 + (temp.e_level / 2));
		temp.e_atk = rnd(sqrt(temp.e_level * 10) * 2);
		temp.e_favoriteAtk = 0; //counter
		temp.e_atkSpd = 1;
		break;
	}
	return temp;
}

void randomEncounter() { //This function creates an encounter randomly for the game
	srand(time(NULL));
	Enemy temp = enemyCreate();
	int numberOfEvents = 4; //The amount of significant events in the game; takes a similarity to parallelism
	int eventEncounter = rand_int(0, 3); //Used in the switch statement to decide which, creates a random value from 0-3

	switch (eventEncounter) {
	case 0: //Player encounters a shop
		shopEncounter();
		break;
	case 1: //Player encounters a boss
		combatNotif();

		if (roomTrack >= sqrt(p_level) * (5 / 2)) {
			bossEncounter();
			killedBoss = true;
		}
		else {
			combat(temp);
		}
		break;
	case 2: //Player encounters a mob, handled by the function mobEncounter
		combatNotif();
		combat(temp);
		break;
	case 3: //Player goes deeper into the dungeon, floor level wise
		if (killedBoss) {
			dungeonProgress();
			roomTrack = 0;
		}
		else {
			shopEncounter();
		}
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
		cin.ignore();
		getline(cin, p_name);
		characterInitialize();
		beginningAdventure();
		break;
	case 0:
		running = false;
	default:
		break;
	}
}

void combat(Enemy mob) {
	do {
		cout << "======================Combat======================\n" << endl;
		cout << setw(5) << left << "Player" << setw(30) << right << "Enemy" << endl;
		cout << setw(5) << left << "Level: " << p_level << setw(30) << right << "Level: " << mob.e_level << endl;
		cout << setw(5) << left << "HP: " << p_hp << '/' << p_hpMax << setw(25) << right << "HP: " << mob.e_hp << '/' << mob.e_hpMax << endl;
		cout << setw(5) << left << "Attack: " << p_atk << setw(30) << right << "Attack: " << mob.e_atk << endl;
		cout << setw(5) << left << "Armor: " << p_armr << setw(30) << right << "Armor: " << mob.e_armr << endl << endl;
		cout << "==================================================\n" << endl;
		cout << "[1] Normal Attack" << endl <<
			"[2] Heavy Attack" << endl <<
			"[3] Counter" << endl <<
			"[4] Consume HP Potion" << endl <<
			"[5] View Enemy Stats" << endl <<
			"[6] View Your Stats" << endl <<
			"[0] Exit Game" << endl <<
			"==================================================\n" << endl <<
			"Input value of your decision:" << endl;

		cin >> cmd;
		system("cls");

		switch (cmd) {
		case 1: //Normal Attack
			p_atkChoice = 1;
			attack(mob, p_atkChoice);
			break;
		case 2: //Heavy Attack
			p_atkChoice = 2;
			attack(mob, p_atkChoice);
			break;
		case 3: //counter
			p_atkChoice = 0;
			attack(mob, p_atkChoice);
			break;
		case 4:
			break;
		case 5:
			printEnemyStats(mob);
			combat(mob);
			break;
		case 6:
			printPlayerStats();
			combat(mob);
			break;
		case 0:
			running = false;
		}

		system("pause");
		system("cls");
	} while ((p_hp > 0) && (mob.e_hp > 0) && (running));


	if (p_hp == 0) {
		tryAgain();
	}
	else {
		endOfCombat;
	}	
}

bool checkState(Enemy mob) {
	if (mob.e_hp == 0) {
		return false;
	}
	if (p_hp == 0) {
		return false;
	}
	return true;
}

void attack(Enemy mob, int p_atkChoice) {
	int p_damage = p_atk * p_atk / (p_atk + mob.e_armr);
	int e_damage = mob.e_atk * mob.e_atk / (mob.e_atk + p_armr);
	do {
		enemyDiceRoll(mob);

		if (doesPlayerWin(mob, p_atkChoice)) {
			cout << "You win this attack phase!\n" << endl <<
				"The enemy has decided to ";
			switch (mob.e_atkChoice) {
			case 0:
				cout << "counter!" << endl <<
					"Your attack penetrates their counter!" << endl;
				mob.e_hp -= p_atk;
				cout << "The enemy has lost " << p_atk << " HP!" << endl;
				break;
			case 1:
				cout << "use a normal attack!" << endl <<
					"You decide to counter and attack!" << endl;
				mob.e_hp -= p_atk;
				cout << "The enemy has lost " << p_atk << " HP!" << endl;
				break;
			case 2:
				cout << "use a heavy attack!" << endl <<
					"The enemy has been dazed!" << endl <<
					"Their attack has been cancelled!" << endl;
				mob.e_hp -= p_atk;
				cout << "The enemy has lost " << p_atk << " HP!" << endl;
				break;
			}
		}
		else {
			cout << "You lose this attack phase!\n" << endl <<
				"You have decided to ";
			switch (p_atkChoice) {
			case 0:
				cout << "counter!" << endl <<
					"The enemy's attack penetrates their counter!" << endl;
				p_hp -= mob.e_atk;
				cout << "You have lost " << mob.e_atk << " HP!" << endl;
				break;
			case 1:
				cout << "use a normal attack!" << endl <<
					"The enemy counters with their counter!" << endl;
				p_hp -= mob.e_atk;
				cout << "You have lost " << mob.e_atk << " HP!" << endl;
				break;
			case 2:
				cout << "use a heavy attack!" << endl <<
					"The enemy uses a normal attack!" << endl <<
					"The enemy dazes you..." << endl <<
					"Your attack has been cancelled!" << endl;
				p_hp -= mob.e_atk;
				cout << "You have lost " << mob.e_atk << " HP!" << endl;
				break;
			}
		}
		system("pause");
		system("cls");

		combat(mob);
	} while ((p_hp > 0) && (mob.e_hp > 0));
}

void enemyDiceRoll(Enemy& mob) {
	int attackRemainSlime[2] = { 1, 2 };
	int attackRemainGoblin[2] = { 0, 2 };
	int attackRemainGolem[2] = { 0, 1 };
	int roll = rand_int(1, 100); 

	if (roll > 75) {
		switch (mob.e_favoriteAtk)
		{
		case 0:
			mob.e_atkChoice = attackRemainSlime[rand_int(0, 1)];
			break;
		case 1: 
			mob.e_atkChoice = attackRemainGoblin[rand_int(0, 1)];
			break;
		case 2:
			mob.e_atkChoice = attackRemainGolem[rand_int(0, 1)];
			break;
		}
	}
	else {
		mob.e_atkChoice = mob.e_favoriteAtk;
	}
}

bool doesPlayerWin(Enemy mob, int p_atkChoice) {
	bool p_firstAttack = false;
	if (p_atkChoice == 0 && mob.e_atkChoice == 1) {
		p_firstAttack = true;
	}
	if (p_atkChoice == 1 && mob.e_atkChoice == 2) {
		p_firstAttack = true;
	}
	if (p_atkChoice == 2 && mob.e_atkChoice == 0) {
		p_firstAttack = true;
	}
	return p_firstAttack;
}

void combatNotif() {
	cout << "==============================\n" << endl <<
		"YOU HAVE ENCOUNTERED AN ENEMY!\n" << endl <<
		"==============================" << endl;

	system("pause");
	system("cls");
}

void endOfCombat(Enemy mob) {
	cout << "Congratulations!\n" << endl <<
		"You have successfully slain a " << mob.e_species << endl << endl;
}

void printEnemyStats(Enemy mob) {
	cout << "======Enemy Stats======\n" << endl <<
		"Species: " << mob.e_species << endl <<
		"Level: " << mob.e_level << endl <<
		"HP: " << mob.e_hp << '/' << mob.e_hpMax << endl << endl <<
		"Attack: " << mob.e_atk << endl <<
		"Favorite Move: ";
	switch (mob.e_favoriteAtk) {
	case 0:
		cout << "Counter" << endl;
		break;
	case 1:
		cout << "Normal Attack" << endl;
		break;
	case 2:
		cout << "Heavy Attack" << endl;
		break;
	}
		cout << "Attack Speed: " << mob.e_atkSpd << endl <<
		"Armor: " << mob.e_armr << endl << endl <<
		"Crit Chance: " << mob.e_critChance << endl <<
		"Dodge Chance: " << mob.e_dodgeChance << endl << endl <<
		"=======================\n" << endl;

	system("pause");
	system("cls");
}

void bossEncounter() {

}

void dungeonProgress() {
	cout << "===========Dungeon Progress===========\n" << endl <<
		"You descend deeper into the dungeon...\n" << endl <<
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
	cout << "This is the item shop" << endl;

	system("pause");
	system("cls");
}

void characterInitialize() {
	int characterTypeHP[3] = { 20, 10, 30 }; //Array for three archtypes of characters
	double characterAtkSpd[3] = { 1, 2, 0.5 }; //Array for the three archtypes' attack speed

	system("cls");
	do {
		cout << "============================\n" << endl <<
			p_name << "... you are...\n" << endl <<
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
			cout << "=============================================================\n" << endl << 
				"Humans have a base health of " << characterTypeHP[cmd - 1] << "..." << endl <<
				"They use normal attacks and have an attack at a speed of " << 1 << "..." << endl << endl
				<< "=============================================================\n" << endl;

			system("pause");
			system("cls");
			confirmCharacter(cmd - 1, characterTypeHP, characterAtkSpd);
			break;
		case 2: //Elf player
			cout << "===================================================\n" << endl << 
				"Elves have a base health of " << characterTypeHP[cmd - 1] << "..." << endl << 
				"Dancing with combat, they attack at a speed of " << 2 << "..." << endl << endl << 
				"===================================================\n" << endl;

			system("pause");
			system("cls");
			confirmCharacter(cmd - 1, characterTypeHP, characterAtkSpd);
			break;
		case 3: //Orc player
			cout << "=====================================================\n" << endl << 
				"Orcs have a base health of " << characterTypeHP[cmd - 1] << "..." << endl << 
				"Using brute strength they attack at a speed of " << 0.5 << "..." <<endl << endl << 
				"=====================================================\n" << endl;

			system("pause");
			system("cls");
			confirmCharacter(cmd - 1, characterTypeHP, characterAtkSpd);
			break;
		}
	} while (!characterConfirm);
}

void confirmCharacter(int n, int arr1[], double arr2[]) {
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
			p_species = static_cast<Species> (n);
			p_hpMax = p_hp = arr1[n];
			p_atk = 5;
			p_atkSpd = arr2[n];
		}
	} while (!characterConfirm);

	system("cls");
}

void printPlayerStats() { //Function that prints all of the character information, used when player wants to view stats
	cout << "=================Player Stats=================\n" << endl;
	cout << "Name: " << p_name << endl;

	cout << "Species: ";
	switch (p_species) {
	case 0: 
		cout << "Human" << endl;
		break;
	case 1: 
		cout << "Elf" << endl;
		break;
	case 2:
		cout << "Orc" << endl;
		break;
	}; 

	cout << "Level: " << p_level << endl;
	cout << "Exp: " << p_exp << '/' << expUpAlg(p_level) << endl;
	cout << "Gold: " << p_gold << endl << endl;
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

	system("pause");
	system("cls");
}

void levelUp(int& level) { //Function that updates the player stats, called whenever the player wants to view stats or finishes a room
	while (p_exp >= p_expUp) {
		p_exp -= expUpAlg(level);
		level++;
		p_expUp = expUpAlg(level);

		cout << "======================LEVEL UP======================" <<
			"You have leveled up from level" << p_level - 1 << " to level" << p_level << endl << endl <<
			"====================================================";
		system("pause");

		specUp();
		specUp();
		statUpdate();
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

void statUpdate() { //Specing into stats are different based on the species that the player choose
	switch (p_species) {
	case 0: 

		break;
	case 1:

		break;
	case 2:

		break;
	}
}

void tryAgain() {
	char ch;
	cout << "==========GAME OVER==========\n" << endl <<
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
