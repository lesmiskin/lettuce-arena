#include <time.h>
#include "scene.h"
#include "enemy.h"
#include "player.h"
#include "lem.h"
#include "input.h"
#include "scene.h"
#include "weapon.h"

bool gameover = false;
bool practice = false;
int fraglimit = 25;
long endTime;
long startTime;
const int START_WAIT = 2000;
const int GAMEOVER_WAIT = 1500;
const int PRACTICE_WAIT = 1500;

void gameOver() {
	endTime = clock();
	gameover = true;
}

void startGame() {
	startTime = clock();
	practice = true;
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

	startGame();
}

void stateFrame() {
	// check for restart
	if(checkCommand(CMD_RESTART)) {
		restartGame();
	}

	// end practice, and start play.
	if(isDue(clock(), startTime, PRACTICE_WAIT)) {
		practice = false;
	}
}