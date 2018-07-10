#include "enemy.h"
#include "time.h"
#include "enemy.h"
#include "player.h"
#include "ai.h"

// ai which does the following.
// moves in an X/Y position to...
// --- get a line of shooting sight with the player.
// --- stands there and shoots until i die.

double getDistance(Coord a, Coord b) {
	// source: http://www.mathwarehouse.com/algebra/distance_formula/index.php
	return sqrt(
		pow(a.x - b.x, 2) + 
		pow(a.y - b.y, 2)
	);
}

void aiSmartFrame(int enemyInc) {

	// shoot at the nearest opponent [done]

	// office theme (worker sprites, chairs, desks with computers)
	// carpet floor sprite

	// realistic walking styles:
	// a) walk in a random direction for a random amount of time, then change direction (officespace-style).
	// c) while walking, 50/50 chance to zig-zag on the way.
	// OR
	// a) pick a random place on the map, and walk to it.


	// reactions:
	// a) if pickup is available, high chance to walk to it.
	// b) if weapon is available, higher chance to walk to it.
	// c) if detect opponent within X threshold (random), then change direction AWAY from him.

	// QUAD DAMAGE powerup spawns infrequently in middle of map.
	// huge gibs fly out when this is used.





	Coord target;
	double bestDistance = 1000;
	Coord usPos = enemies[enemyInc].coord;
	for(int i=0; i < MAX_ENEMY; i++) {
		if(enemies[i].coord.x == 0) continue;
		if(i == enemyInc) continue;				// don't shoot ourselves! :p

		Coord themPos = enemies[i].coord;
		double distance = getDistance(usPos, themPos);

		// we're the shortest so far!
		if(distance < bestDistance) {
			target = themPos;
			bestDistance = distance;
		}
	}

	// give us a chance to target the player too!
	if(getDistance(usPos, pos) < bestDistance) {
		target = pos;
	}

	// shoot at the chosen target
	fireShot(enemyInc, target);
}

void aiChaseFrame(int i) {
	Coord heading;
	bool nowRoaming = false;
	bool skipMove = false;

	//Turn off roaming if enough time has elapsed.
	if(enemies[i].isRoaming && isDue(clock(), enemies[i].lastRoamTime, DIR_CHANGE)) {
		enemies[i].isRoaming = false;
	}

	//If we're roaming - head in that direction.
	if(enemies[i].isRoaming) {
		heading = calcDirOffset(enemies[i].coord, enemies[i].roamDir);
	//Otherwise - home towards player.
	}else{
		Coord homeStep = getStep(enemies[i].coord, pos, ENEMY_SPEED, true);
		heading = deriveCoord(enemies[i].coord, -homeStep.x, -homeStep.y);
	}

	//Loop through all enemies (plus player), and see if we would collide.
	for(int j=-1; j < MAX_ENEMY; j++) {
		//Player check.
		Coord compare = j == -1 ? pos : enemies[j].coord;

		//Don't collide with ourselves :p
		if(j > -1 && i == j) continue;

		//Our heading will put us in the bounds of an enemy. Decide what to do.
		if(inBounds(heading, makeSquareBounds(compare, CHAR_BOUNDS))) {
			//If we're touching the player - stay where we are.
			if(j == -1) {
				skipMove = true;
				break;
			}

			//Only move around 5% of the time (more realistic)
			//TODO: Change to delay?
			if(chance(75)) {
				skipMove = true;
				continue;
			}

			//Try roaming in some other directions to free ourselves.
			bool triedDirs[8] = { false, false, false, false, false, false, false, false };
			Coord roamTarget;
			int tryDir = 0;

			//Keep trying in NSEW dirs until we've exhausted our attempts.
			while(!triedDirs[0] || !triedDirs[1] || !triedDirs[2] || !triedDirs[3] || !triedDirs[4] || !triedDirs[5] || !triedDirs[6] || !triedDirs[7]) {
				//Pick a dir we haven't tried yet.
				do { tryDir = randomMq(0, 7); }
				while(triedDirs[tryDir]);

				triedDirs[tryDir] = true;

				//Would we touch anyone by traveling in this direction?
				roamTarget = calcDirOffset(enemies[i].coord, (Dir)tryDir);
				if(!wouldTouchEnemy(roamTarget, i, false)) {
					enemies[i].roamDir = (Dir)tryDir;
					enemies[i].lastRoamTime = clock();
					enemies[i].isRoaming = true;
					nowRoaming = true;
					break;
				}
			}

			if(nowRoaming) {
				break;
			}else{
				skipMove = true;
				break;
			}
		}
	}

	if(!skipMove) {
		//Move in the direction we're idly roaming in.
		if(enemies[i].isRoaming) {
			enemies[i].coord = calcDirOffset(enemies[i].coord, enemies[i].roamDir);

		//Otherwise, home in on player.
		}else{
			enemies[i].coord = heading;
		}
	}
}
