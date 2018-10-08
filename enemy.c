#include "ai.h"
#include "enemy.h"
#include "lem.h"
#include <time.h>

#define INITIAL_ENEMIES 3
int enemyColorCounter = 1;	// player is always color zero.

char* names[] = { "redwood", "mr elusive", "tokay", "scary", "mynx", "tycho", "lowtax", "hellchick", "cartman", "kenny", "neo", "morpheus" };

void spawnEnemy(Coord point, int color, char* name) {
	// spawn them.
	int lindex = spawnLem(point, color, false, 0, name);

	// set enemy-specific properties.
	lemmings[lindex].angle = randomAngle();
	lemmings[lindex].en_lastDirTime = clock();
	lemmings[lindex].en_nextDirTime = 500;
}

int chosenNames[INITIAL_ENEMIES];
int chosenNameInc = 0;

char* randomName() {
	bool different;
	while(1) {
		int random = randomMq(0,11);
		bool isNameTaken = false;

		// See if we already took this name.
		for(int j=0; j < 7; j++) {
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
	spawns[0] = makeCoord(20, 35);
	spawns[1] = makeCoord(300, 220);
	spawns[2] = makeCoord(20, 220);
	spawns[3] = makeCoord(300, 40);

	// Coord point = spawns[randomMq(0, MAX_SPAWNS)];

	// Make the enemies
	for(int i=0; i < INITIAL_ENEMIES; i++) {
		spawnEnemy(
			spawns[i+1],			// hit spawns in sequence, so we don't telefrag.
			enemyColorCounter++,
			randomName()
		);
	}
}