#ifndef SCENE_H
#define SCENE_H

#include "common.h"
#include "renderer.h"
#include <stdbool.h>

typedef struct {
	int quadrant;
	Coord coord;
} Spawn;

typedef struct {
	int quadrant;
	bool valid;
	bool pickedUp;
	long lastPickup;
	Coord coord;
	int type;
} Weapon;

#define MAP_Y 15
#define MAP_X 20
extern int map[MAP_Y][MAP_X];
extern int getMapTile(int x, int y, int quadrant);

#define MAX_SPAWNS 8
extern Spawn spawns[MAX_SPAWNS];

extern void sceneRenderFrame(void);
extern void sceneAnimateFrame(void);
extern void sceneGameFrame();
extern void initScene();
extern void startScene();
extern Spawn makeSpawn(int quadrant, Coord coord);

#define MAX_WEAPONS 8
extern const int WEAP_ROCKET;
extern Weapon weapons[MAX_WEAPONS];
extern void powerupPickup();

#endif
