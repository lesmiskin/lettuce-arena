#include "enemy.h"
#include "time.h"
#include "player.h"
#include "ai.h"

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
