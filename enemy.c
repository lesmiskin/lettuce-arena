#include "ai.h"
#include "enemy.h"
#include "lem.h"
#include <time.h>

const int INITIAL_ENEMIES = 1;

void spawnEnemy(Coord point, int color) {
	// spawn them.
	int lindex = spawnLem(point, color, false);

	// set enemy-specific properties.
	lemmings[lindex].en_idleTarget = randomAngle();
	lemmings[lindex].en_lastDirTime = clock();
	lemmings[lindex].en_nextDirTime = 500;
}

void initEnemy(void) {
	spawns[0] = makeCoord(20, 20);
	spawns[1] = makeCoord(300, 220);
	spawns[2] = makeCoord(20, 220);
	spawns[3] = makeCoord(300, 20);

	// Coord point = spawns[randomMq(0, MAX_SPAWNS)];

	// Make the enemies
	for(int i=0; i < INITIAL_ENEMIES; i++) {
		spawnEnemy(
			spawns[i+1],			// hit spawns in sequence, so we don't telefrag.
			randomMq(1, 4)
		);
	}
}