#ifndef SCENE_H
#define SCENE_H

#include "common.h"
#include "renderer.h"

#include <stdbool.h>

extern void sceneRenderFrame(void);
extern void sceneAnimateFrame(void);
extern void sceneGameFrame();
extern void initScene();

typedef struct {
	int quadrant;
	bool valid;
	bool pickedUp;
	long lastPickup;
	Coord coord;
	int type;
} Weapon;

#define MAX_WEAPONS 8
extern const int WEAP_ROCKET;
extern Weapon weapons[MAX_WEAPONS];
extern void powerupPickup();

#endif
