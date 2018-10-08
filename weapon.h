#include <stdbool.h>
#include "common.h"

#ifndef WEAP_H
#define WEAP_H

#define MAX_SHOTS 50

extern const double SHOT_RELOAD;

typedef struct {
	bool valid;
    Coord coord;
    Coord target;
    double angle;
    long lastPuff;
    int shooter;
    int animInc;
} Shot;

extern void weaponGameFrame();
extern void weaponRenderFrame();
extern void shoot(int i, double deg);
extern void initWeapon();
extern long lastPlayerKillTime;
extern int lastPlayerKillIndex;

#endif