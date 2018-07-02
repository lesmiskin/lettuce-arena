#ifndef ENEMY_H
#define ENEMY_H

#include "common.h"

#define MAX_SHOTS 50

typedef struct {
    Coord coord;
    Coord target;
    double angle;
} Shot;

typedef enum {
	ENEMY_CTHULU,
	ENEMY_DIGGER,
	ENEMY_WOLFMAN,
	ENEMY_DRACULA,
} EnemyType;

#define MAX_ENEMY 30

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
} Enemy;

extern const double DIR_CHANGE;
extern const double ENEMY_SPEED;
const double CHAR_BOUNDS;

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