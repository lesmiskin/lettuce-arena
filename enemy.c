#include "ai.h"
#include "enemy.h"
#include "lem.h"
#include "player.h"
#include <time.h>
#include "state.h"
#include "scene.h"

#define INITIAL_ENEMIES 3
#define LEMMINGS 4

void spawnEnemy(Spawn spawn, int color, char* name) {
	// spawn them.
	int lindex = spawnLem(spawn, color, false, 0, name);

	// set enemy-specific properties.
	// lemmings[lindex].angle = randomAngle();
	lemmings[lindex].en_lastDirTime = clock();
	lemmings[lindex].en_nextDirTime = 500;
}

#define MAX_COLORS 4
int colors[] = { 0, 1, 2, 3};
int chosenColors[MAX_COLORS];
int chosenColorInc = 0;

int randomColor() {
	bool different;
	while(1) {
		int random = randomMq(0,MAX_COLORS-1);
		bool isColorTaken = false;

		// See if we already took this name.
		for(int j=0; j < INITIAL_ENEMIES; j++) {
			if(random == chosenColors[j]) {
				isColorTaken = true;
				break;
			}
		}

		if(isColorTaken) continue;

		chosenColors[chosenColorInc++] = random;
		return colors[random];
	}
}

#define MAX_NAMES 6
char* names[] = { "sherbert", "blinky", "bob", "whippy", "yorp", "wilkins" };
int chosenNames[INITIAL_ENEMIES];
int chosenNameInc = 0;

char* randomName() {
	bool different;
	while(1) {
		int random = randomMq(0,MAX_NAMES-1);
		bool isNameTaken = false;

		// See if we already took this name.
		for(int j=0; j < INITIAL_ENEMIES; j++) {
			if(random == chosenNames[j]) {
				isNameTaken = true;
				break;
			}
		}

		if(isNameTaken) continue;

		chosenNames[chosenNameInc++] = random;
		return names[random];
	}
}

void initEnemy(void) {
	spawns[0] = makeSpawn(0, makeCoord(20, 35));
	spawns[1] = makeSpawn(0, makeCoord(300, 220));
	spawns[2] = makeSpawn(1, makeCoord(20, 220));
	spawns[3] = makeSpawn(1, makeCoord(300, 40));

	// reset
	for(int i=0; i < INITIAL_ENEMIES; i++) {
		chosenNames[i] = -1;
		chosenColors[i] = -1;
	}

	// Pick a random player spawn point.
	int playerOrder = randomMq(0, LEMMINGS-1);

	// Make the enemies
	for(int i=0; i < LEMMINGS; i++) {
		if(usePlayer && i == playerOrder) {
			playerIndex = spawnLem(spawns[i], randomColor(), true, 0, "spud");
		}else{
			if(DEBUG_NO_ENEMIES) continue;
			spawnEnemy(
				spawns[i],			// hit spawns in sequence, so we don't telefrag.
				randomColor(),
				randomName()
			);
		}
	}
}