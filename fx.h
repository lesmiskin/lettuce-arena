#include <stdbool.h>
#include "common.h"

#ifndef FX_H
#define FX_H

extern void spawnTele(Coord c);
extern void fxGameFrame();
extern void fxRenderFrame();
extern void spawnExp(Coord c, bool smallExp);
extern void spawnExpDelay(Coord c, bool smallExp, int delay);
extern void spawnHurt(Coord c, int lemColor);
extern void spawnLemExp(Coord c, int lemColor);

#define MAX_PUFFS 100
extern const double PUFF_FREQ;
extern void spawnPuff(Coord c);

#endif