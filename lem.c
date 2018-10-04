#include "ai.h"
#include "lem.h"
#include "assets.h"
#include "time.h"
#include "scene.h"
#include "renderer.h"
#include "fx.h"
#include "mysdl.h"
#include "player.h"

Lem lemmings[MAX_LEM];
const int PLAYER_INDEX = 0;
static long lastIdleTime;
static const int ANIM_HZ = 1000 / 4;

int spawnLem(Coord coord, int color, bool isPlayer) {
	spawnTele(coord);

	// player is always at index zero.
	int insertIndex = 0;

	// otherwise find valid insert position in main lemming array.
	if(!isPlayer) {
		for(int i=1; i < MAX_LEM; i++) {
			if(lemmings[i].valid) continue;
			insertIndex	= i;
			break;
		}
	}

	// Set up the LEM object with all his properties.
	Lem l = {
		isPlayer,
		true,
		coord,
		color,
		100,
		1, 				// animInc
		false,			// isWalking

		// weapons
		0,
		false,
		0,

		// dying
		false,
		SDL_FLIP_NONE,
		false,
		0, 
		0, 
		0, 

		// enemy-specific
		0, 
		0, 
		0, 
		0
	};

	// add them
	lemmings[insertIndex] = l;

	return insertIndex;		// pass back index, so client can set further attributes.
}

void updateWalk(int i) {
	// Lemming has stopped.
	if(!lemmings[i].isWalking) {
		lemmings[i].animInc = 2;
		return;
	}

	//Increment animations.
	if(lemmings[i].animInc < 4) {
		lemmings[i].animInc++;
	}else{
		lemmings[i].animInc = 1;
	}
}

void lemGameFrame() {
	// do all animation here.

	for(int i=0; i < MAX_LEM; i++) {
		if(!lemmings[i].valid) continue;

		// Run enemy AI
		if(!lemmings[i].isPlayer){
			aiSmartFrame(i);
		}

		bool prevWalkState = lemmings[i].isWalking;

		// Set variable whether lemming is walking or not
		if(lemmings[i].isPlayer){
			lemmings[i].isWalking = playerWalking;
		}else{
			lemmings[i].isWalking = !havingBreather(i);
		}

		// Trigger immediate animation change if we stopped/started walking.
		if(lemmings[i].isWalking != prevWalkState)
			updateWalk(i);

		// did we pick up a weapon?
		for(int j=0; j < MAX_WEAPONS; j++) {
			if(!weapons[j].valid || weapons[j].pickedUp) continue;

			if(inBounds(lemmings[i].coord, makeSquareBounds(weapons[j].coord, 10))) {
				lemmings[i].hasRock = true;
				lemmings[i].ammo = 5;
				weapons[j].pickedUp = true;
				weapons[j].lastPickup = clock();
			}
		}
	}
}

bool havingBreather(int i) {
	return lemmings[i].en_lastBreather > 0;
}

void lemAnimateFrame() {
	//Animate the enemies
	if(timer(&lastIdleTime, ANIM_HZ)) {
		for(int i=0; i < MAX_LEM; i++) {
			if(!lemmings[i].valid || lemmings[i].dead) continue;

			updateWalk(i);
		}
	}
}

void lemRenderFrame() {
	// show all lemming sprites here.
	for(int i=0; i < MAX_LEM; i++) {
		Lem lem = lemmings[i];
		if(!lem.valid) continue;

		// are we traveling left or right?
		double deg = radToDeg(lem.en_idleTarget);
		SDL_RendererFlip flip = lem.isPlayer ?
			!playerDir : 
			deg > 90 && deg < 270 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

		// draw the sprite

		// Set animation frame on sprite file.
		char frameFile[25];
		sprintf(frameFile, "lem-%s-0%d.png", "red", lem.animInc);

		// draw the sprite
		Sprite lemSprite = makeFlippedSprite(frameFile, flip);
		drawSprite(lemSprite, lem.coord);
	}
}