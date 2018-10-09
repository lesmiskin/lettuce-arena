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

    // weapons
    long lastShot;
    bool hasRock;
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

#define MAX_LEM 4
#define MAX_SPAWNS 4

extern const int PLAYER_INDEX;

extern Coord tryMove(Coord target, Coord origin, int selfIndex);
extern bool canMove(Coord target, int selfIndex);
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