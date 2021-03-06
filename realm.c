<<<<<<< HEAD

/*
Copyright (C) 2014  Frank Duignan

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "console.h"
#include "realm.h"
#include <stdio.h>
#include <time.h>
// Find types: h(ealth),s(trength),m(agic),g(old),w(eapon)
const char FindTypes[]={'h','s','m','g','w'};


// The following arrays define the bad guys and 
// their battle properies - ordering matters!
// Baddie types : O(gre),T(roll),D(ragon),H(ag),K(night)
const char Baddies[]={'O','T','D','H','K'};
// The following is 4 sets of 4 damage types
const byte WeaponDamage[]={10,10,5,25,10,10,5,25,10,15,5,15,5,5,2,10};
#define ICE_SPELL_COST 10
#define FIRE_SPELL_COST 20
#define LIGHTNING_SPELL_COST 30
#define EARTH_SPELL_COST 25
const byte FreezeSpellDamage[]={10,20,5,0};
const byte FireSpellDamage[]={20,10,5,0};
const byte LightningSpellDamage[]={15,10,25,0};
const byte EarthSpellDamage[]={15,10,25,0};
const byte BadGuyDamage[]={10,10,15,5,10};
int GameStarted = 0;
tPlayer thePlayer;
tRealm theRealm;
void delay(int len);

unsigned prbs()
{
	// This is an unverified 31 bit PRBS generator
	// It should be maximum length but this has not been verified 
	static unsigned long shift_register=0xa5a5a5a5;
	static int Initialized=0;
	// This is a mechanism that should pick a different prbs starting 
	// value each run (so long as you don't run in the same second)
	if (!Initialized)
	{
		shift_register=(unsigned long)time(NULL);
		Initialized = 1;
	}


	unsigned long new_bit=0;
	static int busy=0; // need to prevent re-entrancy here
	if (!busy)
	{
		busy=1;
		new_bit= ((shift_register & (1<<27))>>27) ^ ((shift_register & (1<<30))>>30);
		new_bit= ~new_bit;
		new_bit = new_bit & 1;
		shift_register=shift_register << 1;
		shift_register=shift_register | (new_bit);
		busy=0;
	}
	return shift_register & 0x7ffffff; // return 31 LSB's 
}


unsigned range_random(unsigned range)
{
	return prbs() % (range+1);
}
void runGame(void)
{
	char ch;
	
	printString("MicroRealms on the LPC810.");	
	showHelp();		
	while(GameStarted == 0)
	{
		
		showGameMessage("Press S to start a new game");
		ch = getUserInput();			
		
		if ( (ch == 'S') || (ch == 's') )
			GameStarted = 1;
	}
	
	initRealm(&theRealm);	
	initPlayer(&thePlayer,&theRealm);
	showPlayer(&thePlayer);
	showRealm(&theRealm,&thePlayer);
	showGameMessage("Press H for help");
	
	while (1)
	{
		ch = getUserInput();
		ch = ch | 32; // enforce lower case
		switch (ch) {
			case 'h' : {
				showHelp();
				break;
			}
			case 'n' : {
				showGameMessage("North");
				step('n',&thePlayer,&theRealm);
				break;
			}
			case 's' : {
				showGameMessage("South");
				step('s',&thePlayer,&theRealm);
				break;

			}
			case 'e' : {
				showGameMessage("East");
				step('e',&thePlayer,&theRealm);
				break;
			}
			case 'w' : {
				showGameMessage("West");
				step('w',&thePlayer,&theRealm);
				break;
			}
			case '#' : {		
				if (thePlayer.wealth)		
				{
					showRealm(&theRealm,&thePlayer);
					thePlayer.wealth--;
				}
				else
					showGameMessage("No gold!");
				break;
			}
			case 'p' : {				
				showPlayer(&thePlayer);
				break;
			}
		} // end switch
	} // end while
}
void step(char Direction,tPlayer *Player,tRealm *Realm)
{
	int new_x, new_y;
	new_x = Player->x;
	new_y = Player->y;
	byte AreaContents;
	switch (Direction) {
		case 'n' :
		{
			if (new_y > 0)
				new_y--;
			break;
		}
		case 's' :
		{
			if (new_y < MAP_HEIGHT-1)
				new_y++;
			break;
		}
		case 'e' :
		{
			if (new_x <  MAP_WIDTH-1)
				new_x++;
			break;
		}
		case 'w' :
		{
			if (new_x > 0)
				new_x--;
			break;
		}		
	}
	AreaContents = Realm->map[new_y][new_x];
	if ( AreaContents == '*')
	{
		showGameMessage("A rock blocks your path.");
		return;
	}
	Player->x = new_x;
	Player->y = new_y;
	int Consumed = 0;
	switch (AreaContents)
	{
		
		// const char Baddies[]={'O','K','T','B','H'};
		case 'O' :{
			showGameMessage("A smelly green Ogre appears before you");
			Consumed = doChallenge(Player,0);
			break;
		}
		case 'K' :{
			showGameMessage("A Dark Knight charges at you");
			Consumed = doChallenge(Player,4);
			break;
		}
		
		case 'T' :{
			showGameMessage("An evil troll challenges you");
			Consumed = doChallenge(Player,1);
			break;
		}
		case 'D' :{
			showGameMessage("A smouldering Dragon blocks your way !");
			Consumed = doChallenge(Player,2);
			break;
		}
		case 'H' :{
			showGameMessage("A withered hag cackles at you wickedly");
			Consumed = doChallenge(Player,3);
			break;
		}
		case 'h' :{
			showGameMessage("You find an elixer of health");
			setHealth(Player,Player->health+10);
			Consumed = 1;		
			break;
			
		}
		case 's' :{
			showGameMessage("You find a potion of strength");
			Consumed = 1;
			setStrength(Player,Player->strength+1);
			break;
		}
		case 'g' :{
			showGameMessage("You find a shiny golden nugget");
			Player->wealth++;			
			Consumed = 1;
			break;
		}
		case 'm' :{
			showGameMessage("You find a magic charm");
			Player->magic++;						
			Consumed = 1;
			break;
		}
		case 'w' :{
			Consumed = addWeapon(Player,range_random(MAX_WEAPONS-1)+1);
			showPlayer(Player);
			break;			
		}
		case 'X' : {
			// Player landed on the exit
			printString("A door! You exit into a new realm");
			setHealth(Player,100); // maximize health
			initRealm(&theRealm);
			showRealm(&theRealm,Player);
		}
	}
	if (Consumed)
		Realm->map[new_y][new_x] = '.'; // remove any item that was found
}
int doChallenge(tPlayer *Player,int BadGuyIndex)
{
	char ch;
	char Damage;
	const byte *dmg;
	int BadGuyHealth = 100;
	printString("Press F to fight");
	ch = getUserInput() | 32; // get user input and force lower case
	if (ch == 'f')
	{
		printString("Choose action");
		while ( (Player->health > 0) && (BadGuyHealth > 0) )
		{
			// Player takes turn first
			if (Player->magic > ICE_SPELL_COST)
				printString("(I)CE spell");
			if (Player->magic > FIRE_SPELL_COST)
				printString("(F)ire spell");
			if (Player->magic > LIGHTNING_SPELL_COST)
				printString("(L)ightning spell");
			if (Player->magic > EARTH_SPELL_COST)
				printString("(E)arth spell");
			if (Player->Weapon1)
			{
				eputs("(1)Use ");
				printString(getWeaponName(Player->Weapon1));
			}	
			if (Player->Weapon2)
			{
				eputs("(2)Use ");
				printString(getWeaponName(Player->Weapon2));
			}
			printString("(P)unch");
			ch = getUserInput();
			switch (ch)
			{
				case 'i':
				case 'I':
				{
					printString("FREEZE!");
					Player->magic -= ICE_SPELL_COST;
					BadGuyHealth -= FreezeSpellDamage[BadGuyIndex]+range_random(10);
					zap();
					break;
				}
				case 'f':
				case 'F':
				{
					printString("BURN!");
					Player->magic -= FIRE_SPELL_COST;
					BadGuyHealth -= FireSpellDamage[BadGuyIndex]+range_random(10);
					zap();
					break;
				}
				case 'l':
				case 'L':
				{
					printString("ZAP!");
					Player->magic -= LIGHTNING_SPELL_COST;
					BadGuyHealth -= LightningSpellDamage[BadGuyIndex]+range_random(10);
					zap();
					break;
				}
				case 'e':
				case 'E':
				{
					printString("EARTHQUAKE!");
					Player->magic -= EARTH_SPELL_COST;
					BadGuyHealth -= EarthSpellDamage[BadGuyIndex]+range_random(10);
					zap();
					break;
				}
				case '1':
				{
					dmg = WeaponDamage+(Player->Weapon1<<2)+BadGuyIndex;
					printString("Take that!");
					BadGuyHealth -= *dmg + range_random(Player->strength);
					setStrength(Player,Player->strength-1);
					break;
				}
				case '2':
				{
					dmg = WeaponDamage+(Player->Weapon2<<2)+BadGuyIndex;
					printString("Take that!");
					BadGuyHealth -= *dmg + range_random(Player->strength);
					setStrength(Player,Player->strength-1);
					break;
				}
				case 'p':
				case 'P':
				{
					printString("Thump!");
					BadGuyHealth -= 1+range_random(Player->strength);
					setStrength(Player,Player->strength-1);
					break;
				}
				default: {
					printString("You fumble. Uh oh");
				}
			}
			// Bad guy then gets a go 
			
			if (BadGuyHealth < 0)
				BadGuyHealth = 0;
			Damage = BadGuyDamage[BadGuyIndex]+range_random(5);
			setHealth(Player,Player->health - Damage);
			eputs("Health: you "); printHex(Player->health);
			eputs(", them " );printHex(BadGuyHealth);
			eputs("\r\n");
		}
		if (Player->health == 0)
		{ // You died
			printString("You are dead. Press Reset to restart");
			while(1);
		}
		else
		{ // You won!
			Player->wealth = 50 + range_random(50);			
			showGameMessage("You win! Their gold is yours");			
			return 1;
		}
		
	}
	else
	{
		showGameMessage("Our 'hero' chickens out");
		return 0;
	}
}
int addWeapon(tPlayer *Player, int Weapon)
{
	char c;
	eputs("You stumble upon ");
	switch (Weapon)
	{
		case 1:
		{	
			printString("a mighty axe");
			break;
		}
		case 2:
		{	
			printString("a sword with mystical runes");
			break;
		}
		case 3:
		{	
			printString("a bloody flail");
			break;
		}	
		
			printHex(Weapon);
	}
	if ( (Player->Weapon1) && (Player->Weapon2) )
	{
		// The player has two weapons already.
		showPlayer(Player);
		printString("You already have two weapons");		
		printString("(1) drop Weapon1, (2) for Weapon2, (0) skip");
		c = getUserInput();
		switch(c)
		{
			eputc(c);
			case '0':{
				return 0; // don't pick up
			}
			case '1':{
				Player->Weapon1 = Weapon;
				break;
			}
			case '2':{
				Player->Weapon2 = Weapon;
				break;
			}
		}
	}
	else
	{
		if (!Player->Weapon1)
		{
			Player->Weapon1 = Weapon;	
		}
		else if (!Player->Weapon2)
		{
			Player->Weapon2 = Weapon;
		}
	}	
	return 1;
}
const char *getWeaponName(int index)
{
	switch (index)
	{
		case 0:return "Empty"; break;
		case 1:return "Axe";break;
		case 2:return "Sword"; break;
		case 3:return "Flail"; break;
	
	}
}

void setHealth(tPlayer *Player,int health)
{
	if (health > 100)
		health = 100;
	if (health < 0)
		health = 0;
	Player->health = health;
	
}	
void setStrength(tPlayer *Player, byte strength)
{
	if (strength > 100)
		strength = 100;
	if (strength < 0)
		strength = 0;
	Player->strength = strength;
}
void initPlayer(tPlayer *Player,tRealm *theRealm)
{
	// get the player name
	int index=0;
	byte x,y;
	char ch=0;
	// Initialize the player's attributes
	eputs("Enter the player's name: ");
	while ( (index < MAX_NAME_LEN) && (ch != '\n') && (ch != '\r'))
	{
		ch = getchar();//getUserInput();
		if ( ch > '0' ) // strip conrol characters
		{
			Player->name[index++]=ch;
			eputc(ch);
		}
	}
	Player->name[index]=0; // terminate the name
	setHealth(Player,100);
	Player->strength=50+range_random(50);
	Player->magic=50+range_random(50);	
	Player->wealth=10+range_random(10);
	Player->Weapon1 = 0;
	Player->Weapon2 = 0;
	// Initialize the player's location
	// Make sure the player does not land
	// on an occupied space to begin with
	do {
		x=range_random(MAP_WIDTH);
		y=range_random(MAP_HEIGHT);
		
	} while(theRealm->map[y][x] != '.');
	Player->x=x;
	Player->y=y;
}
void showPlayer(tPlayer *thePlayer)
{
	eputs("\r\nName: ");
	printString(thePlayer->name);
	eputs("health: ");
	printHex(thePlayer->health);
	eputs("\r\nstrength: ");
	printHex(thePlayer->strength);
	eputs("\r\nmagic: ");
	printHex(thePlayer->magic);
	eputs("\r\nwealth: ");
	printHex(thePlayer->wealth);	
	eputs("\r\nLocation : ");
	printHex(thePlayer->x);
	eputs(" , ");
	printHex(thePlayer->y);	
	eputs("\r\nWeapon1 : ");
	printString(getWeaponName(thePlayer->Weapon1));
	eputs("Weapon2 : ");
	printString(getWeaponName(thePlayer->Weapon2));
}
void initRealm(tRealm *Realm)
{
	int x,y;
	int Rnd;
	// clear the map to begin with
	for (y=0;y < MAP_HEIGHT; y++)
	{
		for (x=0; x < MAP_WIDTH; x++)
		{
			Rnd = range_random(100);
			
			if (Rnd >= 98) // put in some baddies
				Realm->map[y][x]=	Baddies[range_random(sizeof(Baddies))];
			else if (Rnd >= 95) // put in some good stuff
				Realm->map[y][x]=	FindTypes[range_random(sizeof(FindTypes))];
			else if (Rnd >= 90) // put in some rocks
				Realm->map[y][x]='*'; 
			else // put in empty space
				Realm->map[y][x] = '.';	
		}
	}
	
	// finally put the exit to the next level in
	x = range_random(MAP_WIDTH);
	y = range_random(MAP_HEIGHT);
	Realm->map[y][x]='X';
}
void showRealm(tRealm *Realm,tPlayer *thePlayer)
{
	int x,y;
	printString("The Realm:");	
	for (y=0;y<MAP_HEIGHT;y++)
	{
		for (x=0;x<MAP_WIDTH;x++)
		{
			
			if ( (x==thePlayer->x) && (y==thePlayer->y))
				eputc('@');
			else
				eputc(Realm->map[y][x]);
		}
		eputs("\r\n");
	}
	printString("\r\nLegend");
	printString("(T)roll, (O)gre, (D)ragon, (H)ag,(K)night, e(X)it");
	printString("(w)eapon, (g)old), (m)agic, (s)trength");
	printString("@=You");
}
void showHelp()
{

	printString("Help");
	printString("N,S,E,W : go North, South, East, West");
	printString("# : show map (cost: 1 gold piece)");
	printString("(H)elp");
	printString("(P)layer details");
	
}

void showGameMessage(char *Msg)
{
	printString(Msg);
	printString("Ready");	
}
char getUserInput()
{
	char ch = 0;	
	ch = getchar();
	// need to flush out the input buffer
	int c;
	while ( (c = getchar()) != '\n' && c != EOF ) { }		
	return ch;
}

void zap()
{
	// do some special effect when someone uses a spell
	
}

=======

/*
Copyright (C) 2014  Frank Duignan

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "console.h"
#include "realm.h"
#include <stdio.h>
#include <time.h>
// Find types: h(ealth),s(trength),m(agic),g(old),w(eapon)
const char FindTypes[]={'h','s','m','g','w'};


// The following arrays define the bad guys and 
// their battle properies - ordering matters!
// Baddie types : O(gre),T(roll),D(ragon),H(ag)
const char Baddies[]={'O','T','D','H','K'};
// The following is 4 sets of 4 damage types
const byte WeaponDamage[]={10,10,5,25,10,10,5,25,10,15,5,15,5,5,2,10};
#define ICE_SPELL_COST 10
#define FIRE_SPELL_COST 20
#define LIGHTNING_SPELL_COST 30
const byte FreezeSpellDamage[]={10,20,5,0};
const byte FireSpellDamage[]={20,10,5,0};
const byte LightningSpellDamage[]={15,10,25,0};
const byte BadGuyDamage[]={10,10,15,5};
int GameStarted = 0;
tPlayer thePlayer;
tRealm theRealm;
void delay(int len);

unsigned prbs()
{
	// This is an unverified 31 bit PRBS generator
	// It should be maximum length but this has not been verified 
	static unsigned long shift_register=0xa5a5a5a5;
	static int Initialized=0;
	// This is a mechanism that should pick a different prbs starting 
	// value each run (so long as you don't run in the same second)
	if (!Initialized)
	{
		shift_register=(unsigned long)time(NULL);
		Initialized = 1;
	}


	unsigned long new_bit=0;
	static int busy=0; // need to prevent re-entrancy here
	if (!busy)
	{
		busy=1;
		new_bit= ((shift_register & (1<<27))>>27) ^ ((shift_register & (1<<30))>>30);
		new_bit= ~new_bit;
		new_bit = new_bit & 1;
		shift_register=shift_register << 1;
		shift_register=shift_register | (new_bit);
		busy=0;
	}
	return shift_register & 0x7ffffff; // return 31 LSB's 
}


unsigned range_random(unsigned range)
{
	return prbs() % (range+1);
}
void runGame(void)
{
	char ch;
	
	printString("MicroRealms on the LPC810.");	
	showHelp();		
	while(GameStarted == 0)
	{
		
		showGameMessage("Press S to start a new game");
		ch = getUserInput();			
		
		if ( (ch == 'S') || (ch == 's') )
			GameStarted = 1;
	}
	
	initRealm(&theRealm);	
	initPlayer(&thePlayer,&theRealm);
	showPlayer(&thePlayer);
	showRealm(&theRealm,&thePlayer);
	showGameMessage("Press H for help");
	
	while (1)
	{
		ch = getUserInput();
		ch = ch | 32; // enforce lower case
		switch (ch) {
			case 'h' : {
				showHelp();
				break;
			}
			case 'n' : {
				showGameMessage("North");
				step('n',&thePlayer,&theRealm);
				break;
			}
			case 's' : {
				showGameMessage("South");
				step('s',&thePlayer,&theRealm);
				break;

			}
			case 'e' : {
				showGameMessage("East");
				step('e',&thePlayer,&theRealm);
				break;
			}
			case 'w' : {
				showGameMessage("West");
				step('w',&thePlayer,&theRealm);
				break;
			}
			case '#' : {		
				if (thePlayer.wealth)		
				{
					showRealm(&theRealm,&thePlayer);
					thePlayer.wealth--;
				}
				else
					showGameMessage("No gold!");
				break;
			}
			case 'p' : {				
				showPlayer(&thePlayer);
				break;
			}
		} // end switch
	} // end while
}
void step(char Direction,tPlayer *Player,tRealm *Realm)
{
	int new_x, new_y;
	new_x = Player->x;
	new_y = Player->y;
	byte AreaContents;
	switch (Direction) {
		case 'n' :
		{
			if (new_y > 0)
				new_y--;
			break;
		}
		case 's' :
		{
			if (new_y < MAP_HEIGHT-1)
				new_y++;
			break;
		}
		case 'e' :
		{
			if (new_x <  MAP_WIDTH-1)
				new_x++;
			break;
		}
		case 'w' :
		{
			if (new_x > 0)
				new_x--;
			break;
		}		
	}
	AreaContents = Realm->map[new_y][new_x];
	if ( AreaContents == '*')
	{
		showGameMessage("A rock blocks your path.");
		return;
	}
	Player->x = new_x;
	Player->y = new_y;
	int Consumed = 0;
	switch (AreaContents)
	{
		
		// const char Baddies[]={'O','T','B','H'};
		case 'O' :{
			showGameMessage("A smelly green Ogre appears before you");
			Consumed = doChallenge(Player,0);
			break;
		}
		case 'K' :{
			showGameMessage("A Dark Knight charges at you");
			Consumed = doChallenge(Player,4);
			break;
		}
		
		case 'T' :{
			showGameMessage("An evil troll challenges you");
			Consumed = doChallenge(Player,1);
			break;
		}
		case 'D' :{
			showGameMessage("A smouldering Dragon blocks your way !");
			Consumed = doChallenge(Player,2);
			break;
		}
		case 'H' :{
			showGameMessage("A withered hag cackles at you wickedly");
			Consumed = doChallenge(Player,3);
			break;
		}
		case 'h' :{
			showGameMessage("You find an elixer of health");
			setHealth(Player,Player->health+10);
			Consumed = 1;		
			break;
			
		}
		case 's' :{
			showGameMessage("You find a potion of strength");
			Consumed = 1;
			setStrength(Player,Player->strength+1);
			break;
		}
		case 'g' :{
			showGameMessage("You find a shiny golden nugget");
			Player->wealth++;			
			Consumed = 1;
			break;
		}
		case 'm' :{
			showGameMessage("You find a magic charm");
			Player->magic++;						
			Consumed = 1;
			break;
		}
		case 'w' :{
			Consumed = addWeapon(Player,range_random(MAX_WEAPONS-1)+1);
			showPlayer(Player);
			break;			
		}
		case 'X' : {
			// Player landed on the exit
			printString("A door! You exit into a new realm");
			setHealth(Player,100); // maximize health
			initRealm(&theRealm);
			showRealm(&theRealm,Player);
		}
	}
	if (Consumed)
		Realm->map[new_y][new_x] = '.'; // remove any item that was found
}
int doChallenge(tPlayer *Player,int BadGuyIndex)
{
	char ch;
	char Damage;
	const byte *dmg;
	int BadGuyHealth = 100;
	printString("Press F to fight");
	ch = getUserInput() | 32; // get user input and force lower case
	if (ch == 'f')
	{
		printString("Choose action");
		while ( (Player->health > 0) && (BadGuyHealth > 0) )
		{
			// Player takes turn first
			if (Player->magic > ICE_SPELL_COST)
				printString("(I)CE spell");
			if (Player->magic > FIRE_SPELL_COST)
				printString("(F)ire spell");
			if (Player->magic > LIGHTNING_SPELL_COST)
				printString("(L)ightning spell");
			if (Player->Weapon1)
			{
				eputs("(1)Use ");
				printString(getWeaponName(Player->Weapon1));
			}	
			if (Player->Weapon2)
			{
				eputs("(2)Use ");
				printString(getWeaponName(Player->Weapon2));
			}
			printString("(P)unch");
			ch = getUserInput();
			switch (ch)
			{
				case 'i':
				case 'I':
				{
					printString("FREEZE!");
					Player->magic -= ICE_SPELL_COST;
					BadGuyHealth -= FreezeSpellDamage[BadGuyIndex]+range_random(10);
					zap();
					break;
				}
				case 'f':
				case 'F':
				{
					printString("BURN!");
					Player->magic -= FIRE_SPELL_COST;
					BadGuyHealth -= FireSpellDamage[BadGuyIndex]+range_random(10);
					zap();
					break;
				}
				case 'l':
				case 'L':
				{
					printString("ZAP!");
					Player->magic -= LIGHTNING_SPELL_COST;
					BadGuyHealth -= LightningSpellDamage[BadGuyIndex]+range_random(10);
					zap();
					break;
				}
				case '1':
				{
					dmg = WeaponDamage+(Player->Weapon1<<2)+BadGuyIndex;
					printString("Take that!");
					BadGuyHealth -= *dmg + range_random(Player->strength);
					setStrength(Player,Player->strength-1);
					break;
				}
				case '2':
				{
					dmg = WeaponDamage+(Player->Weapon2<<2)+BadGuyIndex;
					printString("Take that!");
					BadGuyHealth -= *dmg + range_random(Player->strength);
					setStrength(Player,Player->strength-1);
					break;
				}
				case 'p':
				case 'P':
				{
					printString("Thump!");
					BadGuyHealth -= 1+range_random(Player->strength);
					setStrength(Player,Player->strength-1);
					break;
				}
				default: {
					printString("You fumble. Uh oh");
				}
			}
			// Bad guy then gets a go 
			
			if (BadGuyHealth < 0)
				BadGuyHealth = 0;
			Damage = BadGuyDamage[BadGuyIndex]+range_random(5);
			setHealth(Player,Player->health - Damage);
			eputs("Health: you "); printHex(Player->health);
			eputs(", them " );printHex(BadGuyHealth);
			eputs("\r\n");
		}
		if (Player->health == 0)
		{ // You died
			printString("You are dead. Press Reset to restart");
			while(1);
		}
		else
		{ // You won!
			Player->wealth = 50 + range_random(50);			
			showGameMessage("You win! Their gold is yours");			
			return 1;
		}
		
	}
	else
	{
		showGameMessage("Our 'hero' chickens out");
		return 0;
	}
}
int addWeapon(tPlayer *Player, int Weapon)
{
	char c;
	eputs("You stumble upon ");
	switch (Weapon)
	{
		case 1:
		{	
			printString("a mighty axe");
			break;
		}
		case 2:
		{	
			printString("a sword with mystical runes");
			break;
		}
		case 3:
		{	
			printString("a bloody flail");
			break;
		}
  		case 4:
                {
                        printString("a steel hammer");
                        break;
                }		
		default:
			printHex(Weapon);
	}
	if ( (Player->Weapon1) && (Player->Weapon2) )
	{
		// The player has two weapons already.
		showPlayer(Player);
		printString("You already have two weapons");		
		printString("(1) drop Weapon1, (2) for Weapon2, (0) skip");
		c = getUserInput();
		switch(c)
		{
			eputc(c);
			case '0':{
				return 0; // don't pick up
			}
			case '1':{
				Player->Weapon1 = Weapon;
				break;
			}
			case '2':{
				Player->Weapon2 = Weapon;
				break;
			}
		}
	}
	else
	{
		if (!Player->Weapon1)
		{
			Player->Weapon1 = Weapon;	
		}
		else if (!Player->Weapon2)
		{
			Player->Weapon2 = Weapon;
		}
	}	
	return 1;
}
const char *getWeaponName(int index)
{
	switch (index)
	{
		case 0:return "Empty"; break;
		case 1:return "Axe";break;
		case 2:return "Sword"; break;
		case 3:return "Flail"; break;
                case 4:return "Hammer"; break;
	}
}

void setHealth(tPlayer *Player,int health)
{
	if (health > 100)
		health = 100;
	if (health < 0)
		health = 0;
	Player->health = health;
	
}	
void setStrength(tPlayer *Player, byte strength)
{
	if (strength > 100)
		strength = 100;
	if (strength < 0)
		strength = 0;
	Player->strength = strength;
}
void initPlayer(tPlayer *Player,tRealm *theRealm)
{
	// get the player name
	int index=0;
	byte x,y;
	char ch=0;
	// Initialize the player's attributes
	eputs("Enter the player's name: ");
	while ( (index < MAX_NAME_LEN) && (ch != '\n') && (ch != '\r'))
	{
		ch = getchar();//getUserInput();
		if ( ch > '0' ) // strip conrol characters
		{
			Player->name[index++]=ch;
			eputc(ch);
		}
	}
	Player->name[index]=0; // terminate the name
	setHealth(Player,100);
	Player->strength=50+range_random(50);
	Player->magic=50+range_random(50);	
	Player->wealth=10+range_random(10);
	Player->Weapon1 = 0;
	Player->Weapon2 = 0;
	// Initialize the player's location
	// Make sure the player does not land
	// on an occupied space to begin with
	do {
		x=range_random(MAP_WIDTH);
		y=range_random(MAP_HEIGHT);
		
	} while(theRealm->map[y][x] != '.');
	Player->x=x;
	Player->y=y;
}
void showPlayer(tPlayer *thePlayer)
{
	eputs("\r\nName: ");
	printString(thePlayer->name);
	eputs("health: ");
	printHex(thePlayer->health);
	eputs("\r\nstrength: ");
	printHex(thePlayer->strength);
	eputs("\r\nmagic: ");
	printHex(thePlayer->magic);
	eputs("\r\nwealth: ");
	printHex(thePlayer->wealth);	
	eputs("\r\nLocation : ");
	printHex(thePlayer->x);
	eputs(" , ");
	printHex(thePlayer->y);	
	eputs("\r\nWeapon1 : ");
	printString(getWeaponName(thePlayer->Weapon1));
	eputs("Weapon2 : ");
	printString(getWeaponName(thePlayer->Weapon2));
}
void initRealm(tRealm *Realm)
{
	int x,y;
	int Rnd;
	// clear the map to begin with
	for (y=0;y < MAP_HEIGHT; y++)
	{
		for (x=0; x < MAP_WIDTH; x++)
		{
			Rnd = range_random(100);
			
			if (Rnd >= 98) // put in some baddies
				Realm->map[y][x]=	Baddies[range_random(sizeof(Baddies))];
			else if (Rnd >= 95) // put in some good stuff
				Realm->map[y][x]=	FindTypes[range_random(sizeof(FindTypes))];
			else if (Rnd >= 90) // put in some rocks
				Realm->map[y][x]='*'; 
			else // put in empty space
				Realm->map[y][x] = '.';	
		}
	}
	
	// finally put the exit to the next level in
	x = range_random(MAP_WIDTH);
	y = range_random(MAP_HEIGHT);
	Realm->map[y][x]='X';
}
void showRealm(tRealm *Realm,tPlayer *thePlayer)
{
	int x,y;
	printString("The Realm:");	
	for (y=0;y<MAP_HEIGHT;y++)
	{
		for (x=0;x<MAP_WIDTH;x++)
		{
			
			if ( (x==thePlayer->x) && (y==thePlayer->y))
				eputc('@');
			else
				eputc(Realm->map[y][x]);
		}
		eputs("\r\n");
	}
	printString("\r\nLegend");
	printString("(T)roll, (O)gre, (D)ragon, (H)ag,(K)night, e(X)it");
	printString("(w)eapon, (g)old), (m)agic, (s)trength");
	printString("@=You");
}
void showHelp()
{

	printString("Help");
	printString("N,S,E,W : go North, South, East, West");
	printString("# : show map (cost: 1 gold piece)");
	printString("(H)elp");
	printString("(P)layer details");
	
}

void showGameMessage(char *Msg)
{
	printString(Msg);
	printString("Ready");	
}
char getUserInput()
{
	char ch = 0;	
	ch = getchar();
	// need to flush out the input buffer
	int c;
	while ( (c = getchar()) != '\n' && c != EOF ) { }		
	return ch;
}

void zap()
{
	// do some special effect when someone uses a spell
}

>>>>>>> 1eb23f5f8c2454e5c6a7b51e8fe09242c47e24f6
