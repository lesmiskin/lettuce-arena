#ifndef WEAP_H
#define WEAP_H

#include <stdbool.h>
#include "common.h"

#define MAX_SHOTS 50

extern const double MACH_RELOAD;
extern const double ROCK_RELOAD;
extern const double ROCK_DAMAGE;
extern const double MACH_DAMAGE;
extern const double ROCK_PUSH;
extern const double MACH_PUSH;

typedef struct {
	int quadrant;
	bool valid;
    Coord coord;
    Coord target;
    double angle;
    long lastPuff;
    int shooter;
    int animInc;
	int type;
} Shot;

extern int getMaxAmmo(int weap);
extern const int W_ROCK;
extern const int W_MACH;
extern const int I_HEALTH;
extern const int I_AMMO;
extern void weaponGameFrame();
extern void weaponRenderFrame();
extern void shoot(int i, double deg);
extern void initWeapon();
extern long lastPlayerKillTime;
extern int lastPlayerKillIndex;
extern void stopWeap();

#endif