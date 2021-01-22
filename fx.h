#ifndef FX_H
#define FX_H

#include <stdbool.h>
#include "common.h"

extern void spawnTele(Coord c, int quadrant);
extern void fxGameFrame();
extern void fxRenderFrame();
extern void spawnExp(Coord c, bool smallExp, int quadrant);
extern void spawnExpDelay(Coord c, bool smallExp, int delay, int quadrant);
extern void spawnHurt(Coord c, int lemColor, int quadrant);
extern void spawnLemExp(Coord c, int lemColor, int quadrant);

#define MAX_PUFFS 100
extern const double PUFF_FREQ;
extern void spawnPuff(Coord c, int quadrant);
extern void stopFx();

#endif