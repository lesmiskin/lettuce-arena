#include <time.h>
#include "scene.h"
#include "enemy.h"
#include "player.h"
#include "lem.h"
#include "input.h"
#include "scene.h"
#include "weapon.h"
#include "fx.h"

bool usePlayer = true;
bool gameover = false;
bool practice = false;
bool inGame = false;
int fraglimit = 25;
long endTime;
long startTime;
const int GAMEOVER_WAIT = 1500;
const int PRACTICE_WAIT = 1500;

void gameOver() {
	endTime = clock();
	gameover = true;
}

void startMatch() {
	startTime = clock();

	// only bother doing practice if player is there.	
	if(usePlayer && !DEBUG_SKIP_INTRO) 
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

	// TODO: Reconcile these with common.h:changeMode

	initScene();
	initPlayer();
	initEnemy();
	initWeapon();
	gameover = false;

	startMatch();
}

typedef enum {
	INTRO_FALL,
	INTRO_EXP,
	INTRO_SHAKE,
	INTRO_MARCH,
	INTRO_BEGIN
} IntroSequence;

IntroSequence introSeqNumber;
long lastIntroSeqChange = 0;
double logoYPos = -50;
int logoShake = 0;
long lastIntroSeqEvent = 0;
bool introSeqStarted = false;
int SHAKE_AMT = 4;

void startIntro() {
	introSeqNumber = 0;
	introSeqStarted = false;
	lastIntroSeqChange = clock();
}

void nextIntroSeq() {
	introSeqNumber++;
	introSeqStarted = false;
	lastIntroSeqChange = clock();
}

void introFrame() {
	switch(introSeqNumber) {
		case INTRO_FALL:
			// Scene change
			if(logoYPos >= 100) {
				nextIntroSeq();
			} else {
				logoYPos+=1.5;
			}
			break;
		case INTRO_EXP:
			for(int i=0; i < 8; i++) {
				spawnExpDelay(makeCoord(randomMq(130, 190),randomMq(logoYPos, logoYPos+30)), randomMq(0,1), randomMq(0, 50), 0);
			}
			nextIntroSeq();
			break;
		case INTRO_SHAKE:
			// Scene start
			if(!introSeqStarted) {
				viewOffsetY = SHAKE_AMT;
				introSeqStarted = true;
			// Scene change
			}else if(isDue(clock(), lastIntroSeqChange, 250)) {
				nextIntroSeq();
			}else if(timer(&lastIntroSeqEvent, 120)) {
				viewOffsetY = (viewOffsetY == SHAKE_AMT) ? 0 : SHAKE_AMT;
			}
			break;
		case INTRO_MARCH:
			if(!introSeqStarted) {
				viewOffsetY = 0;
				introSeqStarted = true;
			}
			break;
		case INTRO_BEGIN:
			break;
	}

	drawSprite(makeSimpleSprite("la-title.png"), makeCoord(160, logoYPos));
}

void gameFrame() {
	// check for restart
	if(checkCommand(CMD_RESTART)) {
		restartGame();
		return;
	}
	// end practice, and start play.
	if(isDue(clock(), startTime, PRACTICE_WAIT)) {
		practice = false;
	}
}

void stateFrame() {
	switch(currentMode) {
		case MODE_TITLE:
			introFrame();
			break;
		case MODE_GAME:
			gameFrame();
			break;
	}
}
