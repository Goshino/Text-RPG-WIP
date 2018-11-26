#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

using namespace std;

enum Species { //Enum for player species, not necessary
	Human, Elf, Orc
};

struct Enemy { //Stats are initialized to zero before run time, this will be changed at run time
	string e_species;

	int e_level = 0;
	int e_hpMax = 0;
	int e_hp = 0;

	int e_atk = 0;
	int e_favoriteAtk = 0; //0 for counter, 1 for normal, 2 for heavy
	int e_atkChoice = 0; // e_favoriteAtk and e_atkChoice are similar
	int e_armr = 0;

	double e_critDamage = 0;
	int e_expDrop = 0; //Value that determines how much exp the player will gain after finishing an enemy
	int e_goldDrop = 0; //Value that determines how much gold the player will gain after finishing an enemy
};

struct Charm {
	string name;
	int shopBuyPrice = 0; //The amount that is required from the player in order to buy the charm

	//These values will be the bonus that will affect that player's skill points
	int toughness = 0;
	int vitality = 0;
	int strength = 0;
	int dexterity = 0;
	int agility = 0;
};

//Public variables for input and game state
int cmd; //User input

//Reason for this is so that the player does not automatically encounter a boss, scales with player level
static string p_name; //Player name
Species p_species; //Player species, option given in the game
int p_level = 1; //Player level
int p_exp = 0; //Player current exp
int p_expUp = 0; //Exp that is required to level up to the next level, assuming player has 0 exp
int p_gold = 0; //Currency that is gained from killing a mob or boss, used in the item shop to purchase items

Charm charmBag; //Struct of Charm that the player will hold. Player can only hold one charm
int charmBuff = 0; //charmBuff variable impacts 

//These values affect the player, depending on what he or she decided to choose to play as 
int toughness = 0;
int vitality = 0;
int strength = 0;
int dexterity = 0;
int agility = 0;

int p_hpMax = 0; //Player max hp
int p_hp = 0; //Player hp, this is different from max hp as the player is expected to get damaged
int p_hpPotionCount = 0; //This variable holds the amount of hp potions that the player has
int p_atk = 0; //Player damage
int p_atkChoice = 0; //Pllayer's choice of attack from counter, normal, and heavy
double p_critDamage = 0; //Chance for the player to hit a crit; doubles the amount of damage dealt to the mob
int p_armr = 0; //Player's armor stat, used to dedtermine the player's damage resistance

//Public variables for functions
Charm shopCharms[5]; //Global variable for the 5 different types of charms that the player can receive. This refreshes everytime randomEncounter is called
bool killedBoss = false;
bool characterConfirm = false;
int roomTrack = 0; //Tracks the amount of rooms that the player has entered with each dungeon floor


//Prototyping functions, so there won't be chronological conflict
Enemy enemyCreate(); //Enemy struct that randomly generates an enemy
Enemy bossCreate(); //This function creates a boss, that is significantly different from enemyCreate();

void combat(Enemy mob); //combat(Enemy mob); as well as attack(Enemy mob, int p_atkChoice); handles the attack portion of the game
void attack(Enemy mob, int p_atkChoice);
bool checkBothHP(Enemy mob); //Functions from lines 84-87 are used for combat
void checkWinner(Enemy mob);
void enemyDiceRoll(Enemy mob);
bool doesPlayerWin(Enemy mob, int p_atkChoice);
void combatNotif(); //Regular enemy prompt
void bossPrompt(); //Boss enemy prompt

void charmBonus(Charm charm); //Function that calculates the bonus that the player receives from his or her charm
void printCharmInfo(const Charm& charm);  //Function that prints the necessary charm information
void purchaseShopCharm(const Charm& charm); //Function that gives the player the ability to purchase a charm
Charm randomizeCharm(int charmType); //Function that randomly generates charms
void buyCharm(const Charm& charm); //Function that confirms the player whether he or she wants to buy the charm
void freeCharm(Charm charm); //Function that is used to give the player a free charm, used in the randomizedLoot function
void randomizeLoot();//Function that randomizes loot for the player if he or she decides that option, used with dungeonProgress
void potionBuy();//Function that asks that checks and make sure if the user wants to buy a potion


//Room Types
void rest(); //Function that allows the player to rest without the use of health potions. This is a room type
void randomEncounter(); //Function that forces a random encounter whenever the player enters a room: shopkeeper, rest, mob, boss, and progression in dungeon level
void shopEncounter(); //Generates a shop for the player
void dungeonProgress(); //Function where the player progresses further into the dungeon level wise

void beginningAdventure(); //Function that actually begins the adventure for the player
void beginningMenu(); //UI for beginning menu, prompts the rest of the game

void characterInitialize(); //Function that initializes player stat variables for the start of the game
void confirmCharacter(int n, int arr[]); //Function used in characterInitialize, used to confirm the character stats

void printEnemyStats(Enemy mob); //Function that prints the enemy's stats
void printPlayerStats(); //Function that shows the player his or her stats 

void levelUp(int& level); //Function that updates the player's stats updated after a completion of every room
void confirmSpec(); //Function that prompts the user to let him or her know what he or she put it in
void specUp(); //Function that allows the user where he or she wants to put his or her skill 
void statUpdate(int spec); //Function that updates the player's stats, used whenver there is a posiblity of the player leveling

int rnd(double n); //Function for rounding
int expUpAlg(int level); //Function that decides the amount needed to level up the player
int rand_int(int low, int high); //Function that easily translates the rand % x algorithm into a function, not necessary but nice to have

void tryAgain(); //Function that allows the game to be run again
void resetStats(); //Function that resets all global variables

//Main
void main() {
	srand(time(NULL)); //Random seed
	beginningMenu(); //beginingMenu function, nested function handles the game from here
}

//Definitions
Enemy enemyCreate() { //Math heavy in order to give different mob types different hp
	int type = rand_int(0, 2); //Creates a random number, used in the switch case below to randomly generate a regular enemy
	Enemy temp;
	temp.e_level = rand_int(p_level, p_level + 5); //Makes it so that mobs can spawn +5 levels above the player for added difficulty 
	temp.e_goldDrop = rand_int(1, 100); //Randomizes gold drom
	switch (type) { //Switch statement is used here to decide what to type of enemy to create. Different enemy types spawn with different stats
	case 0: //Goblins
		temp.e_species = "Goblin";
		temp.e_hp = temp.e_hpMax = rnd(sqrt(temp.e_level) * 10 + (temp.e_level / 3));
		temp.e_expDrop = (temp.e_level * 5) + rnd(temp.e_level / 2);
		temp.e_atk = rnd(sqrt(temp.e_level * 10) * 1.5);
		temp.e_favoriteAtk = 1; //Normal attack
		temp.e_armr = rand_int(0, rnd(p_armr * 0.5));
		break;
	case 1: //Golems
		temp.e_species = "Golem";
		temp.e_hp = temp.e_hpMax = rnd(sqrt(temp.e_level) * 10) + temp.e_level;
		temp.e_expDrop = (temp.e_level * 5) + (2 * temp.e_level);
		temp.e_atk = rnd(sqrt(temp.e_level * 10) * 2);
		temp.e_favoriteAtk = 2; //Heavy Attack
		temp.e_armr = rand_int(p_armr, rnd(p_armr * 1.5));
		break;
	case 2: //Slimes
		temp.e_species = "Slime";
		temp.e_hp = temp.e_hpMax = rnd(sqrt(temp.e_level) * 10 + (temp.e_level / 2));
		temp.e_expDrop = (temp.e_level * 5) + rnd(temp.e_level / 2);
		temp.e_atk = rnd(sqrt(temp.e_level * 10) * 1.5);
		temp.e_favoriteAtk = 0; //counter
		temp.e_armr = rand_int(rnd(p_armr * 0.5), p_armr);
		break;
	}
	return temp;
}

Enemy bossCreate() { //Similar to the enemyCreate function, however this generates a boss, with an insane difficulty
	//Not as math heavy as the boss is supposed to be a reflection of the player's stats. Really really difficult to clear this boss
	//Instead of scaling to level, this is scaled in complement to the player
	int type = rand_int(0, 2);
	Enemy temp;
	temp.e_level = p_level; //Sets boss level to enemy's
	temp.e_goldDrop = rand_int(100, 10000); //Gives a lot of gold compared to regular mobs as this is a boss
	temp.e_expDrop = (expUpAlg(p_level) * 100);
	temp.e_atk = p_hpMax; //Makes it so the boss can one shot the player
	temp.e_critDamage = p_critDamage; //Makes it so that boss has critDamage that is on par with the player
	switch (type) {
	case 0: //Goblins
		temp.e_species = "Goblin";
		temp.e_hp = temp.e_hpMax = p_hpMax * 10;
		temp.e_armr = p_armr;
		temp.e_favoriteAtk = 1;
		break;
	case 1: //Golems
		temp.e_species = "Golem";
		temp.e_hp = temp.e_hpMax = p_hpMax * 30;
		temp.e_armr = p_armr * 4;
		temp.e_favoriteAtk = 2;
		break;
	case 2: //Slimes
		temp.e_species = "Slime";
		temp.e_hp = temp.e_hpMax = p_hpMax * 20;
		temp.e_armr = p_armr * 2;
		temp.e_favoriteAtk = 0;
		break;
	}
	return temp;
}

void rest() { //Function that alllows the player to gain hp without the means of hp potions. Heals for half health
	int recover = rnd(.50 * p_hpMax);
	cout << "==================Rest Site=================\n" << endl <<
		"You feel refreshed and have recovered HP..." << endl <<
		"You have recovered " << recover << "HP\n" << endl <<
		"============================================\n";

	p_hp += recover;
	if (p_hp > p_hpMax) {
		p_hp = p_hpMax;
	}

	system("pause");
	system("cls");

	randomEncounter();
}

void randomEncounter() { //This function creates an encounter randomly for the game
	roomTrack++;
	Enemy temp = enemyCreate();
	Enemy boss = bossCreate();
	for (size_t i = 0; i < 5; i++) {
		shopCharms[i] = randomizeCharm(i + 1);
	}

	int eventEncounter = rand_int(0, 4); //Used in the switch statement to decide which, creates a random value from 0-3

	switch (eventEncounter) {
	case 0: //Player encounters a shop
		shopEncounter();
		break;
	case 1: //Player encounters a boss
		if (roomTrack >= (rnd(sqrt(p_level)) * (5 / 2))) { //Logic that is used so that the player does not encounter a boss within the first room of the floor
			bossPrompt();
			combat(boss);
			killedBoss = true; //sets bool killedBoss to true, so that the player now has the possibility to progress deeper into the dungeon
		}
		else {
			combatNotif();
			combat(temp);
		}
		break;
	case 2: //Player encounters a mob, handled by the function
		combatNotif();
		combat(temp);
		break;
	case 3: //Player goes deeper into the dungeon, floor level wise
		if (killedBoss) {
			dungeonProgress();
		}
		else {
			combat(temp);
		}
		break;
	case 4: //Rest
		if (p_hp == p_hpMax) {
			shopEncounter();
		}
		else {
			rest();
		}
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
		exit(0);
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
		exit(0);
	}
}

void combat(Enemy mob) {
	int healAmount = rnd(p_hpMax * 0.25);

	while (checkBothHP(mob)) {
		cout << "======================Combat======================\n" << endl;
		cout << setw(5) << left << "Player" << setw(30) << right << "Enemy" << endl;
		cout << setw(5) << left << "Level: " << p_level << setw(30) << right << "Level: " << mob.e_level << endl;
		cout << setw(5) << left << "HP: " << p_hp << '/' << p_hpMax << setw(25) << right << "HP: " << mob.e_hp << '/' << mob.e_hpMax << endl;
		cout << setw(5) << left << "Attack: " << p_atk << setw(30) << right << "Attack: " << mob.e_atk << endl;
		cout << setw(5) << left << "Armor: " << p_armr << setw(30) << right << "Armor: " << mob.e_armr << endl << endl;
		cout << "==================================================\n" << endl <<
			"[1] Normal Attack" << endl <<
			"[2] Heavy Attack" << endl <<
			"[3] Counter" << endl <<
			"[4] Consume HP Potion" << endl <<
			"[5] View Enemy Stats" << endl <<
			"[6] View Your Stats" << endl <<
			"[0] Exit Game\n" << endl <<
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
			if (p_hpPotionCount == 0) {
				cout << "===============Error!===============\n" << endl <<
					"You do not have enough HP potions!\n" << endl <<
					"====================================" << endl;

				system("pause");
				system("cls");
				continue;
			}
			else {
				p_hpPotionCount--;
				cout << "======================Health Potion Used!=====================\n" << endl <<
					"You have used a potion and healed for " << healAmount << " HP" << endl <<
					"You have " << p_hpPotionCount << " potions remaining\n" << endl <<
					"==============================================================" << endl;
				p_hp += healAmount;
				if (p_hp > p_hpMax) {
					p_hp = p_hpMax;
				}

				system("pause");
				system("cls");
				continue;
			}
		case 5:
			printEnemyStats(mob);
			continue;
		case 6:
			printPlayerStats();
			continue;
		case 0:
			exit(0);
		}
	}
}

void attack(Enemy mob, int p_atkChoice) {
	system("cls");
	int p_tempAtk;
	int e_tempAtk;
	int roll = rand_int(1, 100);

	if (roll <= 50) {
		p_tempAtk = rnd(dexterity * 0.01 * p_atk) + p_atk;
		e_tempAtk = rnd(mob.e_critDamage * 0.01 * mob.e_atk) + mob.e_atk;
	}
	else {
		p_tempAtk = p_atk;
		e_tempAtk = mob.e_atk;
	}

	int p_damage = p_tempAtk * p_tempAtk / (p_tempAtk + mob.e_armr);
	int e_damage = e_tempAtk * e_tempAtk / (e_tempAtk + p_armr);

	while (checkBothHP(mob)) {
		enemyDiceRoll(mob);

		cout << "====================Combat====================\n" << endl;

		if (doesPlayerWin(mob, p_atkChoice)) {
			cout << "You win this attack phase!\n" << endl <<
				"The enemy has decided to ";
			switch (mob.e_atkChoice) {
			case 0:
				cout << "counter!" << endl <<
					"Your attack penetrates their counter!" << endl;
				mob.e_hp -= p_damage;
				cout << "The enemy has lost " << p_damage << " HP!" << endl;
				break;
			case 1:
				cout << "use a normal attack!" << endl <<
					"You decide to counter and attack!" << endl;
				mob.e_hp -= p_damage;
				cout << "The enemy has lost " << p_damage << " HP!" << endl;
				break;
			case 2:
				cout << "use a heavy attack!" << endl <<
					"The enemy has been dazed!" << endl <<
					"Their attack has been cancelled!" << endl;
				mob.e_hp -= p_damage;
				cout << "The enemy has lost " << p_damage << " HP!" << endl;
				break;
			}
		}
		else {
			cout << "You lose this attack phase!\n" << endl <<
				"You have decided to ";
			switch (p_atkChoice) {
			case 0:
				cout << "counter!" << endl <<
					"The enemy's attack penetrates your counter!" << endl;
				p_hp -= e_damage;
				cout << "You have lost " << e_damage << " HP!" << endl;
				break;
			case 1:
				cout << "use a normal attack!" << endl <<
					"The enemy counters with their counter!" << endl;
				p_hp -= e_damage;
				cout << "You have lost " << e_damage << " HP!" << endl;
				break;
			case 2:
				cout << "use a heavy attack!" << endl <<
					"The enemy uses a normal attack!" << endl <<
					"The enemy dazes you..." << endl <<
					"Your attack has been cancelled!" << endl;
				p_hp -= e_damage;
				cout << "You have lost " << e_damage << " HP!" << endl;
				break;
			}
		}
		cout << "\n==============================================\n" << endl;

		system("pause");
		system("cls");

		combat(mob);
	}
	checkWinner(mob);
}

bool checkBothHP(Enemy mob) {
	if (p_hp <= 0) {
		return false;
	}
	if (mob.e_hp <= 0) {
		return false;
	}
	return true;
}

void checkWinner(Enemy mob) {
	if (p_hp <= 0) {
		tryAgain();
	}
	else {
		cout << "================Congratulations!===================\n" << endl <<
			"You have successfully slain a " << mob.e_species << endl << endl <<
			"===================================================" << endl;

		system("pause");
		system("cls");

		p_exp += mob.e_expDrop;
		p_gold += mob.e_goldDrop;

		cout << "======================Room Cleared=====================\n" << endl <<
			"You have gained " << mob.e_expDrop << " experience points!" << endl <<
			"You have gained " << mob.e_goldDrop << " gold!\n" << endl <<
			"======================================================" << endl;

		system("pause");
		system("cls");

		levelUp(p_level);

		randomEncounter();
	}
}

void enemyDiceRoll(Enemy mob) {
	int attackRemainSlime[2] = { 1, 2 }; //Normal, Heavy
	int attackRemainGoblin[2] = { 0, 2 }; //Block, Heavy
	int attackRemainGolem[2] = { 0, 1 }; //Block, Normal
	int roll = rand_int(1, 4);
	if (roll == 1) {
		switch (mob.e_favoriteAtk) {
		case 0: //Counter user
			mob.e_atkChoice = attackRemainSlime[rand() % 1];
			break;
		case 1://Normal User
			mob.e_atkChoice = attackRemainGoblin[rand() % 1];
			break;
		case 2://Heavy user
			mob.e_atkChoice = attackRemainGolem[rand() % 1];
			break;
		}
	}
	else {
		mob.e_atkChoice = mob.e_favoriteAtk;
	}
}

bool doesPlayerWin(Enemy mob, int p_atkChoice) {
	bool p_firstAttack;
	int fiftyFifty = rand_int(0, 1);
	if (p_atkChoice == 0 && mob.e_atkChoice == 1) {
		p_firstAttack = true;
	}
	else if (p_atkChoice == 1 && mob.e_atkChoice == 2) {
		p_firstAttack = true;
	}
	else if (p_atkChoice == 2 && mob.e_atkChoice == 0) {
		p_firstAttack = true;
	}
	else {
		(fiftyFifty == 0) ? p_firstAttack = true : p_firstAttack = false;
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

void printEnemyStats(Enemy mob) {
	cout << "===========Enemy Stats===========\n" << endl <<
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
	cout << "Armor: " << mob.e_armr << endl << endl <<
		"Crit Damage: " << mob.e_critDamage << endl <<
		"=================================\n" << endl;

	system("pause");
	system("cls");
}

void bossPrompt() {
	cout << "==============Warning=============\n" << endl <<
		"You have entered a boss room!\n" << endl <<
		"==================================" << endl;

	system("pause");
	system("cls");
}

void dungeonProgress() {
	roomTrack = 0;
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
		levelUp(p_level);
		break;
	case 2:
		randomizeLoot();
		break;
	}
	randomEncounter();
}

void freeCharm(Charm charm) {
	cout << "==============Free Charm=============\n" << endl <<
		"You have found a charm!" << endl <<
		"[1] View charm" << endl <<
		"[2] View current charm" << endl <<
		"[3] Switch charm" << endl <<
		"[4] Proceed to next room\n" << endl <<
		"=====================================";

	cin >> cmd;
	system("cls");

	switch (cmd) {
	case 1:
		printCharmInfo(charm);
		break;
	case 2:
		printCharmInfo(charmBag);
		break;
	case 3:
		charmBag = charm;
		cout << "===========Charm Switch===========\n" << endl <<
			"You have switched your charm!\n" << endl <<
			"==================================" << endl;

		system("pause");
		system("cls");
		break;
	case 4:
		break;
	}
	randomEncounter();
}

void randomizeLoot() {
	int randLoot = rand_int(1, 7);
	int gold = rand_int(100, 200);
	int randPot = rand_int(1, 10);
	switch (randLoot) {
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		freeCharm(shopCharms[randLoot]);
		break;
	case 6:
		cout << "=========Free Potions==========\n" << endl <<
			"You have gained " << randPot << " HP potions\n" << endl <<
			"===============================";

		p_hpPotionCount += randPot;

		system("pause");
		system("cls");
		break;
	case 7:
		cout << "==============Free Gold=============\n" << endl <<
			"You have gained " << gold << " gold!\n" << endl <<
			"====================================" << endl;

		p_gold += gold;

		system("pause");
		system("cls");
		break;
	}
}

Charm randomizeCharm(int charmType) {
	Charm temp;
	switch (charmType) {
	case 1:
		temp.name = "Toughness Charm";
		temp.toughness = rand_int(1, 100);
		temp.shopBuyPrice = temp.toughness * 20;
		break;
	case 2:
		temp.name = "Vitality Charm";
		temp.vitality = rand_int(1, 100);
		temp.shopBuyPrice = temp.vitality * 20;
		break;
	case 3:
		temp.name = "Strength Charm";
		temp.strength = rand_int(1, 100);
		temp.shopBuyPrice = temp.strength * 20;
		break;
	case 4:
		temp.name = "Dexterity Charm";
		temp.dexterity = rand_int(1, 100);
		temp.shopBuyPrice = temp.dexterity * 20;
		break;
	case 5:
		temp.name = "Agility Charm";
		temp.agility = rand_int(1, 100);
		temp.shopBuyPrice = temp.agility * 20;
		break;
	}
	return temp;
}

void charmBonus(Charm charm) {
	if (charm.name == "Toughness Charm") {
		toughness += charm.toughness * toughness;
	}
	else if (charm.name == "Vitality Charm") {
		vitality += charm.vitality * vitality;
	}
	else if (charm.name == "Strength Charm") {
		strength += charm.strength * strength;
	}
	else if (charm.name == "Dexterity Charm") {
		dexterity += charm.dexterity * dexterity;
	}
	else {
		agility += charm.agility * agility;
	}
}

void printCharmInfo(const Charm& charm) {
	cout << "==============Charm Info============\n" << endl <<
		"Type: " << charm.name << endl <<
		"Buy Price: " << charm.shopBuyPrice << endl;
	if (charm.toughness >= 0) {
		cout << "Strength Bonus: +" << charm.toughness << '%' << endl;
	}
	if (charm.vitality >= 0) {
		cout << "Vitality Bonus: +" << charm.vitality << '%' << endl;
	}
	if (charm.strength >= 0) {
		cout << "Strength Bonus: +" << charm.strength << '%' << endl;
	}
	if (charm.dexterity >= 0) {
		cout << "Dexterity Bonus: +" << charm.dexterity << '%' << endl;
	}
	if (charm.agility >= 0) {
		cout << "Agility Bonus: +" << charm.agility << '%' << endl;
	}
	cout << "\n====================================" << endl;

	system("pause");
	system("cls");

}

void purchaseShopCharm(const Charm& charm) {
	printCharmInfo(charm);
	buyCharm(charm);
}

void buyCharm(const Charm& charm) {
	char ch;
	cout << "=======================================\n" << endl <<
		"Would you like to buy this charm? (Y/N)\n" << endl <<
		"=======================================\n" << endl <<
		"Input value of your decision:" << endl;

	cin >> ch;
	system("cls");

	if ((ch == 'y') || (ch == 'Y')) {
		if (p_gold >= charm.shopBuyPrice) {
			charmBag = charm;
		}
		else {
			cout << "==========Error!=========\n" << endl <<
				"Insufficient Funds!\n" << endl <<
				"=========================" << endl;
			system("pause");
			system("cls");
			shopEncounter();
		}
	}
	else {
		shopEncounter();
	}
}

void potionBuy() {
	char ch;
	cout << "======================Potions====================\n" << endl <<
		"Would you like to buy a potion for 10 gold? (Y/N)\n" << endl <<
		"=================================================\n" << endl <<
		"Input value of your decision: " << endl;

	cin >> ch;
	system("cls");

	if (ch == 'y' || ch == 'Y') {
		if (p_gold >= 10) {
			p_gold -= 10;
			p_hpPotionCount++;


			cout << "================Purchase===============\n" << endl <<
				"You have purchased a potion for 10 gold\n" << endl <<
				"=======================================" << endl;

			system("pause");
			system("cls");
		}
		else {
			cout << "==========Error!=========\n" << endl <<
				"Insufficient funds!\n" << endl <<
				"=========================" << endl;

			system("pause");
			system("cls");
		}
	}
	shopEncounter();
}

void shopEncounter() {
	cout << "=================Item Shop================\n" << endl <<
		"Gold: " << p_gold << endl <<
		"Select what you would like to purchase:\n" << endl <<
		"[1] Toughness Charm " << endl <<
		"[2] Vitality Charm" << endl <<
		"[3] Strength Charm " << endl <<
		"[4] Dexterity Charm " << endl <<
		"[5] Agility Charm " << endl <<
		"[6] HP Potions " << endl <<
		"[7] Proceed to next room\n" << endl <<
		"==========================================\n" << endl <<
		"Input value of your decision: " << endl;

	cin >> cmd;
	system("cls");

	switch (cmd) {
	case 1: //Toughness Charm
		purchaseShopCharm(shopCharms[0]);
		break;
	case 2: //Vitality Charm
		purchaseShopCharm(shopCharms[1]);
		break;
	case 3:	//Strength Charm
		purchaseShopCharm(shopCharms[2]);
		break;
	case 4: //Dexterity Charm
		purchaseShopCharm(shopCharms[3]);
		break;
	case 5: //Agility Charm
		purchaseShopCharm(shopCharms[4]);
		break;
	case 6:
		potionBuy();
		break;
	case 7:
		randomEncounter();
		break;
	}
}

void characterInitialize() {
	int characterTypeHP[3] = { 30, 20, 40 }; //Array for three archtypes of characters

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
			cout << "==================================\n" << endl <<
				"Humans have a base health of " << characterTypeHP[cmd - 1] << "..." << endl << endl <<
				"==================================\n" << endl;

			system("pause");
			system("cls");
			confirmCharacter(cmd - 1, characterTypeHP);
			break;
		case 2: //Elf player
			cout << "=================================\n" << endl <<
				"Elves have a base health of " << characterTypeHP[cmd - 1] << "..." << endl << endl <<
				"=================================\n" << endl;

			system("pause");
			system("cls");
			confirmCharacter(cmd - 1, characterTypeHP);
			break;
		case 3: //Orc player
			cout << "================================\n" << endl <<
				"Orcs have a base health of " << characterTypeHP[cmd - 1] << "..." << endl << endl <<
				"================================\n" << endl;

			system("pause");
			system("cls");
			confirmCharacter(cmd - 1, characterTypeHP);
			break;
		}
	} while (!characterConfirm);
}

void confirmCharacter(int n, int arr[]) {
	char ch;
	do {
		cout << "=============================\n" << endl <<
			"Are you sure? (Y/N)\n" << endl <<
			"=============================\n" << endl <<
			"Input value of your decision:" << endl;
		cin >> ch;
		(ch == 'y') || (ch == 'Y') ? characterConfirm = true : characterConfirm = false;
		if (!characterConfirm) {
			characterInitialize();
		}
		else {
			p_species = static_cast<Species> (n);
			p_hpMax = p_hp = arr[n];
			p_atk = 5;
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
	cout << "Armor: " << p_armr << endl << endl;
	cout << "Crit Damage: " << dexterity * 0.02 << '%' << endl;
	cout << "Toughness: " << toughness << endl;
	cout << "Vitality: " << vitality << endl;
	cout << "Strength: " << strength << endl;
	cout << "Dexterity: " << dexterity << endl;
	cout << "Agility: " << agility << endl << endl;
	cout << "==============================================" << endl;

	system("pause");
	system("cls");
}

void levelUp(int& level) { //Function that updates the player stats, called whenever the player wants to view stats or finishes a room
	while (p_exp >= p_expUp) {
		p_exp -= expUpAlg(level);
		level++;
		p_expUp = expUpAlg(level);

		cout << "======================LEVEL UP======================\n" << endl <<
			"You have leveled up from level " << p_level - 1 << " to level " << p_level << endl << endl <<
			"====================================================" << endl;
		system("pause");
		system("cls");

		specUp();
		specUp();
		charmBonus(charmBag);
	}
}

void confirmSpec() {
	cout << "========================Spec Confirm========================\n" << endl <<
		"You have successfully put your a skill point in ";
	switch (cmd) {
	case 1:
		cout << "toughness" << endl;
		break;
	case 2:
		cout << "vitality" << endl;
		break;
	case 3:
		cout << "strength" << endl;
		break;
	case 4:
		cout << "dexterity" << endl;
		break;
	case 5:
		cout << "agility" << endl;
		break;
	}
	cout << "\n============================================================" << endl;

	system("pause");
	system("cls");
}

void specUp() {
	cout << "======================================\n" << endl <<
		"Decide where to put your skill points\n" << endl <<
		"[1] Toughness" << endl <<
		"[2] Vitality" << endl <<
		"[3] Strength" << endl <<
		"[4] Dexterity" << endl <<
		"[5] Agility\n" << endl <<
		"======================================\n" << endl <<
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

	confirmSpec();
	statUpdate(cmd);
}

void statUpdate(int spec) { //Specing into stats are different based on the species that the player choose
	switch (p_species) {
	case 0: // Human
		p_hp = p_hpMax = rnd(sqrt(p_level) * 10) + rnd(2 * p_level / 3) + vitality + strength + 30;
		p_atk = rnd(sqrt(p_level * 10)) + rnd(p_level / 10) + agility + strength;
		p_armr = rnd(p_hpMax / 10) + toughness;
		break;
	case 1: //Elf
		p_hp = p_hpMax = rnd(sqrt(p_level) * 10) + rnd(2 * p_level / 3) + rnd(vitality / 4) + rnd(strength / 2) + 20;
		p_atk = rnd(sqrt(p_level * 10)) + rnd(p_level / 10) + (agility * 4) + rnd(strength / 2);
		p_armr = rnd(p_hpMax / 10) + rnd(toughness / 4);

		break;
	case 2: //Orc
		p_hp = p_hpMax = rnd(sqrt(p_level) * 10) + rnd(2 * p_level / 3) + (vitality * 4) + (strength * 2) + 40;
		p_atk = rnd(sqrt(p_level * 10)) + rnd(p_level / 10) + rnd(agility / 4) + (strength * 2);
		p_armr = rnd(p_hpMax / 10) + (toughness * 4);
		break;
	}
}

void resetStats() {
	p_level = 1;
	p_exp = 0;
	p_expUp = 0;
	p_gold = 0;

	toughness = 0;
	vitality = 0;
	strength = 0;
	dexterity = 0;
	agility = 0;
	charmBuff = 0;

	p_hpMax = 0;
	p_hp = 0;
	p_hpPotionCount = 0;
	p_atk = 0;
	p_atkChoice = 0;
	p_critDamage = 0;
	p_armr = 0;

	killedBoss = false;
	characterConfirm = false;
	roomTrack = 0;
}

void tryAgain() { //This function is called if the player falls to fatal hp (0)
	bool runChoice;
	char ch;
	cout << "==========GAME OVER==========\n" << endl <<
		"Would you like to try again? (Y/N)\n" << endl <<
		"=============================\n" << endl <<
		"Input value of your decision: " << endl;

	cin >> ch;
	system("cls");

	((ch == 'Y') || (ch == 'y')) ? runChoice = true : runChoice = false; //If player chooses to continue, call the function bellow
	if (runChoice) {
		beginningMenu();
		resetStats();
	}
	else { //Else stop
		exit(0);
	}
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
