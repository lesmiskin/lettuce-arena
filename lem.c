#include "ai.h"
#include "lem.h"
#include "assets.h"
#include "time.h"
#include "scene.h"
#include "renderer.h"
#include "fx.h"
#include "mysdl.h"
#include "player.h"
#include "weapon.h"

Coord spawns[MAX_SPAWNS];

Lem lemmings[MAX_LEM];
const int PLAYER_INDEX = 0;
static long lastIdleTime;
static const int ANIM_HZ = 1000 / 4;
const double LEM_SPEED = 1;
const double LEM_BOUND = 15;

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
		!isPlayer,
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
	//Animate the lemmings
	if(timer(&lastIdleTime, ANIM_HZ)) {
		for(int i=0; i < MAX_LEM; i++) {
			if(!lemmings[i].valid || lemmings[i].dead) continue;

			updateWalk(i);
		}
	}
}

void weaponCarryFrame(int i) {
	char file[13];
	Coord derive;
	double angle = 0;
	int xoff, yoff;

	// weapon rotation and position.
	switch((int)radToDeg(lemmings[i].angle)+90) {
		case 360:
			sprintf(file, "w_rock-n.png");
			derive = makeCoord(1, -5);
			break;
		case 90:
			sprintf(file, "w_rock-e.png");
			derive = makeCoord(3, 1);
			break;
		case 270:
			sprintf(file, "w_rock-w.png");
			derive = makeCoord(-3, 1);
			break;
		case 180:
			sprintf(file, "w_rock-s.png");
			derive = makeCoord(-4, 5);
			break;

		case 405://ne
			sprintf(file, "w_rock-ne.png");
			derive = makeCoord(4, -3);
			break;
		case 135://se
			sprintf(file, "w_rock-se.png");
			derive = makeCoord(2, 3);
			break;
		case 225://sw
			sprintf(file, "w_rock-sw.png");
			derive = makeCoord(-2, 3);
			break;
		default:
		case 305://nw
			sprintf(file, "w_rock-nw.png");
			derive = makeCoord(-4, -3);
			break;
	}

	// weapon bob cycle.
	switch((int)radToDeg(lemmings[i].angle)+90) {
		// n/s bobbing
		case 360:
		case 180:
			if(lemmings[i].animInc == 1) { xoff = -1; yoff = 1; }	// backstep
			if(lemmings[i].animInc == 2) { xoff = 0;  yoff = 0; }	// up
			if(lemmings[i].animInc == 3) { xoff = 1;  yoff = 1; }	// forwardstep
			if(lemmings[i].animInc == 4) { xoff = 0;  yoff = 0; }	// up
			break;

		// east bobbing
		case 90:
		case 405://ne
		case 135://se
			if(lemmings[i].animInc == 1) { xoff = 1; yoff = 2; }	// backstep
			if(lemmings[i].animInc == 2) { xoff = 0;  yoff = 0; }	// up
			if(lemmings[i].animInc == 3) { xoff = -1;  yoff = 2; }	// forwardstep
			if(lemmings[i].animInc == 4) { xoff = 0;  yoff = 0; }	// up
			break;

		// west bobbing
		case 270:
		case 225://sw
		case 305://nw
		default:
			if(lemmings[i].animInc == 1) { xoff = -1; yoff = 2; }	// backstep
			if(lemmings[i].animInc == 2) { xoff = 0;  yoff = 0; }	// up
			if(lemmings[i].animInc == 3) { xoff = 1;  yoff = 2; }	// forwardstep
			if(lemmings[i].animInc == 4) { xoff = 0;  yoff = 0; }	// up
			break;
	}

	// draw weapon
	Coord wc = deriveCoord(lemmings[i].coord, derive.x+xoff, derive.y+yoff);
	drawSprite(makeSimpleSprite(file), wc);
}

bool canShoot(int i) {
	Lem l = lemmings[i];
	return l.ammo > 0 && l.hasRock && isDue(clock(), l.lastShot, SHOT_RELOAD);
}

void lemRenderFrame() {
	// show all lemming sprites here.
	for(int i=0; i < MAX_LEM; i++) {
		Lem lem = lemmings[i];
		if(!lem.valid) continue;

		// are we traveling left or right?
		double deg = radToDeg(lem.angle);
		SDL_RendererFlip flip = deg > 90 && deg < 270 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
//		printf(lemmings[0].angle);

		// Set animation frame on sprite file.
		char frameFile[25];
		sprintf(frameFile, "lem-%s-0%d.png", "red", lem.animInc);

		// draw the sprite
		Sprite lemSprite = makeFlippedSprite(frameFile, flip);
		drawSprite(lemSprite, lem.coord);

		// draw carrying weapon
		if(lem.hasRock) 
			weaponCarryFrame(i);

		// draw player plume
		if(lem.isPlayer)
			drawSprite(makeSimpleSprite("p1.png"), deriveCoord(lem.coord, -1, -13));
	}
}