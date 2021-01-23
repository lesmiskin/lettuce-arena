#ifndef LEM_H
#define LEM_H

#include <stdbool.h>
#include "common.h"
#include "scene.h"

typedef struct {
	int quadrant;
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
	long lastPickup;
	Coord pickupCoord;
	char* lastItem;

    // weapons
    long lastShot;
	bool shotFirst; // for recoil.
    bool hasRock;
	int weap;
	int lastWeap;
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
	long en_lastDecision;
	int en_nextDecisionTime;
	int en_decision;

	double pushAngle;
	double pushAmount;
} Lem;

// IMPORTANT AI DECISIONS
// const int DC_IDLE = 0;	 			// longer periods of walking.
// const int DC_FIND_WEAPON = 1; 		// if no weapon, do this.
// const int DC_FIND_OPPONENT = 2; 	// noone onscreen? go and find them.
// const int DC_FRESH_QUADRANT = 3; 	// been here too long, find somewhere new.

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

#define MAX_LEM 8

extern const int PLAYER_INDEX;

extern int getReloadTime(int i);
extern Move tryMove(Coord target, Coord origin, int selfIndex);
extern void respawn(int color);
extern Lem lemmings[MAX_LEM];
extern void lemGameFrame();
extern void lemRenderFrame();
extern int spawnLem(Spawn spawn, int color, bool isPlayer, int frags, char* name);
extern void lemAnimateFrame();
extern bool havingBreather(int i);
extern bool canShoot(int i);
extern const double LEM_SPEED;
extern const double LEM_BOUND;
extern void respawn(int i);
extern void stopLem();

#endif