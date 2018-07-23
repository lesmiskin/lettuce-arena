#ifndef ENEMY_H
#define ENEMY_H

#include "common.h"

#define MAX_SHOTS 100
#define MAX_ENEMY 15

typedef struct {
	bool valid;
	Coord coord;
	int animInc;
	long lastShot;
	double idleTarget;
	long lastDirTime;
	int nextDirTime;
	long lastBreather;
	int color;
    bool dead;
    SDL_RendererFlip corpseDir;
    bool buried;
    int deadInc;
    int starInc;
    long buriedTime;
    // he's buried.
    // now that he's buried, vanish him after a while.
} Enemy;

typedef struct {
	bool valid;
    Coord coord;
    Coord target;
    double angle;
    long lastPuff;
    int shooter;
    int animInc;
} Shot;

extern const double ENEMY_SPEED;
extern const double CHAR_BOUNDS;
extern const double SHOT_RELOAD;

extern void enemyDeathRenderFrame();
extern void enemyFxRenderFrame();
extern void enemyFxFrame();
extern bool canShoot(int enemyIndex);
extern bool havingBreather(int enemyInc);
extern double randomEnemyAngle();
extern bool onScreen(Coord coord, double threshold);
extern void fireAngleShot(int enemyIndex, double deg);
extern Enemy enemies[MAX_ENEMY];
extern void enemyGameFrame(void);
extern void enemyRenderFrame(void);
extern void initEnemy(void);
extern void enemyAnimateFrame(void);
extern Shot shots[MAX_SHOTS];
extern bool wouldTouchEnemy(Coord a, int selfIndex, bool includePlayer);

#endif
