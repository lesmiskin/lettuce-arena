#ifndef PLAYER_H
#define PLAYER_H

#include "common.h"

extern void playerGameFrame(void);
extern void playerShadowFrame(void);
extern void initPlayer();
extern void startPlayer();
extern int health;
extern bool playerWalking;
extern bool playerDir;
extern int playerIndex;

#endif
