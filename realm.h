<<<<<<< HEAD
// realm.h
// Some game constants
#define MAP_WIDTH 30
#define MAP_HEIGHT 20
#define MAX_NAME_LEN 20
#define MAX_WEAPONS 25


typedef unsigned char byte;
typedef struct {
	byte map[MAP_HEIGHT][MAP_WIDTH];
	
} tRealm;
typedef struct {
	char name[MAX_NAME_LEN+1];
	byte health;	
	byte strength;
	byte magic;
	byte wealth;
	byte x,y;
	byte Weapon1;
	byte Weapon2;
} tPlayer;

// Function prototypes

void showHelp();
void showGameMessage(char *Msg);
char getUserInput();
void runGame(void);
void initRealm(tRealm *Realm);
void showRealm(tRealm *Realm,tPlayer *thePlayer);
void initPlayer(tPlayer *Player,tRealm *Realm);
void showPlayer(tPlayer *thePlayer);
void step(char Direction,tPlayer *Player,tRealm *Realm);
void setHealth(tPlayer *Player,int health);
void setStrength(tPlayer *Player, byte strength);
int addWeapon(tPlayer *Player, int Weapon);
int doChallenge(tPlayer *Player, int BadGuyIndex);
const char *getWeaponName(int index);
void zap(void);
=======
// realm.h
// Some game constants
#define MAP_WIDTH 30
#define MAP_HEIGHT 20
#define MAX_NAME_LEN 20
#define MAX_WEAPONS 10


typedef unsigned char byte;
typedef struct {
	byte map[MAP_HEIGHT][MAP_WIDTH];
	
} tRealm;
typedef struct {
	char name[MAX_NAME_LEN+1];
	byte health;	
	byte strength;
	byte magic;
	byte wealth;
	byte x,y;
	byte Weapon1;
	byte Weapon2;
} tPlayer;

// Function prototypes

void showHelp();
void showGameMessage(char *Msg);
char getUserInput();
void runGame(void);
void initRealm(tRealm *Realm);
void showRealm(tRealm *Realm,tPlayer *thePlayer);
void initPlayer(tPlayer *Player,tRealm *Realm);
void showPlayer(tPlayer *thePlayer);
void step(char Direction,tPlayer *Player,tRealm *Realm);
void setHealth(tPlayer *Player,int health);
void setStrength(tPlayer *Player, byte strength);
int addWeapon(tPlayer *Player, int Weapon);
int doChallenge(tPlayer *Player, int BadGuyIndex);
const char *getWeaponName(int index);
void zap(void);
>>>>>>> 1eb23f5f8c2454e5c6a7b51e8fe09242c47e24f6
