#ifndef ENEMY_H
#define ENEMY_H

#include "common.h"

// #define MAX_SHOTS 50
#define MAX_ENEMY 8

typedef struct {
	bool valid;
	Coord coord;
	int animInc;
	long lastShot;
	double idleTarget;				// ai
	long lastDirTime;				// ai
	int nextDirTime;				// ai
	long lastBreather;				// ai
	int color;
    bool dead;
    SDL_RendererFlip corpseDir;
    bool stunned;
    int deadInc;
    int starInc;
    long stunnedTime;
    bool hasRock;
    int health;
    int ammo;
    // he's buried.
    // now that he's buried, vanish him after a while.
} Enemy;

// typedef struct {
// 	bool valid;
//     Coord coord;
//     Coord target;
//     double angle;
//     long lastPuff;
//     int shooter;
//     int animInc;
// } Shot;

extern const double MIN_DIR_CHANGE;
extern const double MAX_DIR_CHANGE;

extern const double ENEMY_SPEED;
extern const double CHAR_BOUNDS;
extern const double SHOT_RELOAD;

extern void enemyDeathRenderFrame();
extern void enemyFxRenderFrame();
extern void enemyFxFrame();
// extern bool havingBreather(int enemyInc);
extern double randomAngle();
extern bool onScreen(Coord coord, double threshold);
extern void fireAngleShot(int e, double deg);
extern Enemy enemies[MAX_ENEMY];
extern void enemyGameFrame(void);
extern void enemyRenderFrame(void);
extern void initEnemy(void);
extern void enemyAnimateFrame(void);
extern Shot shots[MAX_SHOTS];
extern bool wouldTouchEnemy(Coord a, int selfIndex, bool includePlayer);

#endif
