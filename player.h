#ifndef PLAYER_H
#define PLAYER_H

#include "common.h"

extern void playerGameFrame(void);
extern void playerRenderFrame(void);
extern void playerShadowFrame(void);
extern void initPlayer();
extern Coord pos;
extern void playerAnimateFrame(void);
extern int health;
extern bool playerWalking;
extern bool playerDir;

#endif
