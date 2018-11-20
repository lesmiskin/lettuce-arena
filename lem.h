#include <stdbool.h>
#include "common.h"

#ifndef LEM_H
#define LEM_H

typedef struct {
	int killer;
	char* name;
	int frags;
	long lastFlash;
	bool flashInc;
	long spawnTime;
	long spawnTimeSdl;
	bool active;
	long deadTime;
    bool isPlayer;
	bool isEnemy;
    bool valid;
    Coord coord;
    int color;
    int health;
    int animInc;
    bool isWalking;
	long lastHit;

    // weapons
    long lastShot;
	bool shotFirst; // for recoil.
    bool hasRock;
	int weap;
    int ammo;

    // dying
    bool dead;
    SDL_RendererFlip corpseDir;
    bool stunned;
    int deadInc;
    int starInc;
    long stunnedTime;

    // enemy properties
    double angle;
    long en_lastDirTime;
    int en_nextDirTime;
    long en_lastBreather;

	double pushAngle;
	double pushAmount;
} Lem;

typedef struct {
	bool up;
	bool down;
	bool left;
	bool right;
} Dir4;

typedef struct {
	Coord result;
	Dir4 freeDir;
} Move;

#define MAX_LEM 4
#define MAX_SPAWNS 4

extern const int PLAYER_INDEX;

extern int getReloadTime(int i);
extern Move tryMove(Coord target, Coord origin, int selfIndex);
extern void respawn(int color);
extern Lem lemmings[MAX_LEM];
extern void lemGameFrame();
extern void lemRenderFrame();
extern int spawnLem(Coord coord, int color, bool isPlayer, int frags, char* name);
extern void lemAnimateFrame();
extern bool havingBreather(int i);
extern bool canShoot(int i);
extern const double LEM_SPEED;
extern const double LEM_BOUND;
extern void respawn(int i);

extern Coord spawns[MAX_SPAWNS];

#endif