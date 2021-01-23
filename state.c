#include <time.h>
#include "scene.h"
#include "enemy.h"
#include "player.h"
#include "lem.h"
#include "input.h"
#include "scene.h"
#include "weapon.h"
#include "fx.h"
#include "hud.h"
#include "assets.h"

bool usePlayer = true;
bool gameover = false;
bool practice = false;
bool inGame = false;
int fraglimit = 25;
long endTime;
long startTime;
const int GAMEOVER_WAIT = 1500;
const int PRACTICE_WAIT = 1500;
bool introReady = false;


// INTRO SEQUENCE
typedef enum {
	INTRO_FALL,
	INTRO_EXP,
	INTRO_SHAKE,
	INTRO_BEGIN
} IntroSequence;

IntroSequence introSeqNumber;
bool introSeqStarted = false;
long lastIntroSeqChange = 0;
long introShakeTime = 0;
long introTextFlashTime = 0;
double logoYPos = 0;
double logo2YPos = 0;
double cosIncLogo = 0;
double sineIncLogo = 0;
const int SHAKE_AMT = 4;
bool introTextFlash = false;

void startIntro() {
	introSeqNumber = 0;
	introSeqStarted = false;
	lastIntroSeqChange = clock();
	introShakeTime = 0;
	introTextFlashTime = 0;
	cosIncLogo = 0;
	sineIncLogo = 0;
	logoYPos = 0;
	introReady = false;
}

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

void nextIntroSeq() {
	introSeqNumber++;
	introSeqStarted = false;
	lastIntroSeqChange = clock();
}

void introFrame() {
	switch(introSeqNumber) {
		case INTRO_FALL:
			if(!introSeqStarted) {
				play("fall.wav");
				introSeqStarted = true;
			}

			// Scene change
			if(logoYPos >= 91) {
				nextIntroSeq();
			} else {
				logoYPos = 150 + cosInc(0, &cosIncLogo, 0.02, 0.25) * 600;
				logo2YPos = 235 + sineInc(0, &sineIncLogo, 0.02, 0.25) * 600;
			}
			break;
		case INTRO_EXP:
			play("expdeath.wav");
			spawnExpDelay(makeCoord(130, randomMq(logoYPos+5, logoYPos+7)), 0, randomMq(0, 50), 0);
			spawnExpDelay(makeCoord(150, randomMq(logoYPos+5, logoYPos+7)), randomMq(0, 1), randomMq(0, 50), 0);
			spawnExpDelay(makeCoord(170, randomMq(logoYPos+5, logoYPos+7)), randomMq(0, 1), randomMq(0, 50), 0);
			spawnExpDelay(makeCoord(190, randomMq(logoYPos+5, logoYPos+7)), 0, randomMq(0, 50), 0);
			nextIntroSeq();
			break;
		case INTRO_SHAKE:
			// Scene start
			if(!introSeqStarted) {
				viewOffsetY = SHAKE_AMT;
				introSeqStarted = true;
			// Scene change
			}else if(isDue(clock(), lastIntroSeqChange, 300)) {
				nextIntroSeq();
			}else if(timer(&introShakeTime, 120)) {
				viewOffsetY = (viewOffsetY == SHAKE_AMT) ? 0 : SHAKE_AMT;
			}
			introReady = true;
			break;
		case INTRO_BEGIN:
			break;
	}

	if(introReady){
		if(timer(&introTextFlashTime, 100)) {
			introTextFlash = !introTextFlash;
		}
		if(introTextFlash) {
			writeFontFull("press any key to start", makeCoord(121, 200), false, false);
		}
	}

	drawSprite(makeSimpleSprite("la-title-a.png"), makeCoord(160, logoYPos));
	drawSprite(makeSimpleSprite("la-title-b.png"), makeCoord(160, logo2YPos));
}

void gameFrame() {
	// Game start
	if(checkCommand(CMD_START)) {
		changeMode(MODE_GAME);
	// check for restart
	} else if(checkCommand(CMD_RESTART)) {
		restartGame();
	// end practice, and start play.
	}else if(isDue(clock(), startTime, PRACTICE_WAIT)) {
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
