#include <time.h>
#include "scene.h"
#include "enemy.h"
#include "player.h"
#include "lem.h"
#include "input.h"
#include "scene.h"
#include "weapon.h"

bool gameover = false;
int fraglimit = 1;
long endTime;
const int GAMEOVER_WAIT = 1500;

void gameOver() {
	endTime = clock();
	gameover = true;
}

void restartGame() {
	// wait for a few seconds so we don't accidentally quit from endgame
	if(!isDue(clock(), endTime, GAMEOVER_WAIT)) {
		return;
	}

	for(int i=0; i < MAX_LEM; i++) {
		lemmings[i].valid = false;
	}

	initScene();
	initPlayer();
	initEnemy();
	initWeapon();
	gameover = false;
}

void stateFrame() {
	if(checkCommand(CMD_RESTART)) {
		restartGame();
	}
}