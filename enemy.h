#ifndef ENEMY_H
#define ENEMY_H

#include "common.h"

#define MAX_SHOTS 100
#define MAX_ENEMY 30

typedef struct {
	bool valid;
    Coord coord;
    Coord target;
    double angle;
    long lastPuff;
    int shooter;
} Shot;

typedef enum {
	ENEMY_CTHULU,
	ENEMY_DIGGER,
	ENEMY_WOLFMAN,
	ENEMY_DRACULA,
} EnemyType;

typedef enum {
	DIR_NORTH = 0,
	DIR_NORTHEAST = 1,
	DIR_EAST = 2,
	DIR_SOUTHEAST = 3,
	DIR_SOUTH = 4,
	DIR_SOUTHWEST = 5,
	DIR_WEST = 6,
	DIR_NORTHWEST = 7
} Dir;

typedef struct {
	Coord coord;
	int animInc;
	EnemyType type;
	long lastRoamTime;
	Dir roamDir;
	bool isRoaming;
	long lastShot;
	double idleTarget;
	long lastDirTime;
	int nextDirTime;
	long lastBreather;
	int color;
} Enemy;

extern const double DIR_CHANGE;
extern const double ENEMY_SPEED;
extern const double CHAR_BOUNDS;
extern const double SHOT_RELOAD;

extern void enemyFxRenderFrame();
extern void enemyFxFrame();
extern bool canShoot(int enemyIndex);
extern bool havingBreather(int enemyInc);
extern double randomEnemyAngle();
extern bool onScreen(Coord coord, double threshold);
extern void fireAngleShot(int enemyIndex, double deg);
extern Coord calcDirOffset(Coord original, Dir dir);
extern Enemy enemies[MAX_ENEMY];
extern void enemyGameFrame(void);
extern void enemyRenderFrame(void);
extern void initEnemy(void);
extern void enemyAnimateFrame(void);
extern void spawnEnemy(EnemyType type, Coord coord);
extern Shot shots[MAX_SHOTS];
extern bool wouldTouchEnemy(Coord a, int selfIndex, bool includePlayer);

#endif
