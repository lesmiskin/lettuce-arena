#include "ai.h"
#include "enemy.h"
#include "lem.h"
#include "player.h"
#include <time.h>
#include "state.h"
#include "scene.h"

#define INITIAL_ENEMIES 7

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
	// Have to comment this out, as we *need* to reuse colors for >4 players.

	// bool different;
	// while(1) {
		int random = randomMq(0,MAX_COLORS-1);
	// 	bool isColorTaken = false;

	// 	// See if we already took this name.
	// 	for(int j=0; j < INITIAL_ENEMIES; j++) {
	// 		if(random == chosenColors[j]) {
	// 			isColorTaken = true;
	// 			break;
	// 		}
	// 	}

	// 	if(isColorTaken) continue;

	// 	chosenColors[chosenColorInc++] = random;
		return colors[random];
	// }
}

#define MAX_NAMES 8
char* names[] = { "sherbert", "blinky", "bob", "whippy", "yorp", "wilkins", "figgy", "duggie" };
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
	spawns[1] = makeSpawn(0, makeCoord(20, 220));
	spawns[2] = makeSpawn(0, makeCoord(300, 35));
	spawns[3] = makeSpawn(0, makeCoord(300, 220));

	spawns[4] = makeSpawn(1, makeCoord(20, 35));
	spawns[5] = makeSpawn(1, makeCoord(20, 220));
	spawns[6] = makeSpawn(1, makeCoord(300, 35));
	spawns[7] = makeSpawn(1, makeCoord(300, 220));

	// reset
	for(int i=0; i < INITIAL_ENEMIES; i++) {
		chosenNames[i] = -1;
		chosenColors[i] = -1;
	}

}

void startEnemy() {
	// Pick a random player spawn point.
	int playerOrder = randomMq(0, MAX_LEM-1);

	// Make the enemies
	for(int i=0; i < MAX_LEM; i++) {
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