#include <stdbool.h>
#include "common.h"

#ifndef LEM_H
#define LEM_H

typedef struct {
    bool isPlayer;
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
    double en_idleTarget;
    long en_lastDirTime;
    int en_nextDirTime;
    long en_lastBreather;
} Lem;

#define MAX_LEM 4

extern const int PLAYER_INDEX;

extern Lem lemmings[MAX_LEM];
extern void lemGameFrame();
extern void lemRenderFrame();
extern int spawnLem(Coord coord, int color, bool isPlayer);
extern void lemAnimateFrame();
extern bool havingBreather(int i);

#endif