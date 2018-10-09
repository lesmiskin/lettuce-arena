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
#include "hud.h"
#include "state.h"

Coord spawns[MAX_SPAWNS];

const int RESPAWN_TIME = 1500;

Lem lemmings[MAX_LEM];
const int PLAYER_INDEX = 0;
static long lastIdleTime;
static const int ANIM_HZ = 1000 / 4;
const double LEM_SPEED = 1;
const double LEM_BOUND = 15;

int spawnLem(Coord coord, int color, bool isPlayer, int frags, char* name) {
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
		-1,
		name,
		frags,
		clock(),
		0,
		clock(),
		true,
		0,
		isPlayer,
		!isPlayer,
		true,
		coord,
		color,
		3,				// health
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

		// SDL_RendererFlip flip = deg > 90 && deg < 270 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

		coord.x > screenBounds.x/2 ? 3.14 : 0, 	// face inwards if on right side of screen
		0, 
		0, 
		0,

		0,
		0
	};

	// add them
	lemmings[insertIndex] = l;

	return insertIndex;		// pass back index, so client can set further attributes.
}

void respawn(int i) {
	if(gameover) return;

	// take out of main rotation.
	lemmings[i].valid = false;

	spawnLem(
		spawns[randomMq(0,3)],
		lemmings[i].color,
		lemmings[i].isPlayer,
		lemmings[i].frags,
		lemmings[i].name
	);
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

	if(practice) return;

	for(int i=0; i < MAX_LEM; i++) {
		if(!lemmings[i].valid) continue;

		// Respawn if dead
		if(lemmings[i].dead && isDue(clock(), lemmings[i].deadTime, RESPAWN_TIME)) {
			respawn(i);
		}

		if(!lemmings[i].active) continue;

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
				lemmings[i].ammo += 3;
				weapons[j].pickedUp = true;
				weapons[j].lastPickup = clock();
			}
		}

		// blast pushing
		if(lemmings[i].pushAmount > 0.5) {
			Coord step = getAngleStep(lemmings[i].pushAngle, lemmings[i].pushAmount, false);
			lemmings[i].pushAmount /= 1.25;
			lemmings[i].coord.x += step.x;
			lemmings[i].coord.y += step.y;
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
		if(!lem.valid || !lem.active) continue;

		// are we traveling left or right?
		double deg = radToDeg(lem.angle);
		SDL_RendererFlip flip = deg > 90 && deg < 270 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
//		printf(lemmings[0].angle);

		char frameFile[25];

		// Set animation frame on sprite file.
		switch(lem.color) {
			case 0:
				sprintf(frameFile, "lem-%s-0%d.png", "red", lem.animInc);
				break;
			case 1:
				sprintf(frameFile, "lem-%s-0%d.png", "blue", lem.animInc);
				break;
			case 2:
				sprintf(frameFile, "lem-%s-0%d.png", "pink", lem.animInc);
				break;
			case 3:
				sprintf(frameFile, "lem-%s-0%d.png", "orange", lem.animInc);
				break;
		}

		bool showName = !lem.isPlayer;

		// draw player plume
		if(lem.isPlayer){
			// spawn "you are here" signal upon respawn.
			if(!isDue(clock(), lem.spawnTime, practice ? PRACTICE_WAIT : 1000)) {
				if(isDue(clock(), lem.lastFlash, 100)) {
					lemmings[i].flashInc = !lemmings[i].flashInc;
					lemmings[i].lastFlash = clock();
				}
				if(lem.flashInc) {
					drawSprite(makeSimpleSprite("flash.png"), lem.coord);
					writeFontFull(lem.name, deriveCoord(lem.coord, 0, -18), false, true);
//					drawSprite(makeSimpleSprite("p1.png"), deriveCoord(lem.coord, -1, -15));
				}
			} else {
				showName = true;
//				drawSprite(makeSimpleSprite("p1-arrow.png"), deriveCoord(lem.coord, -1, -13));
			}
		}

		// show angle as plume.		
		// writeAmount(radToDeg(lem.angle), deriveCoord(lem.coord, 0, -18));

		if(showName) {
			writeFontFull(lem.name, deriveCoord(lem.coord, 0, -18), false, true);
		}

		// draw the sprite
		Sprite lemSprite = makeFlippedSprite(frameFile, flip);
		drawSprite(lemSprite, lem.coord);

		// draw carrying weapon
		if(lem.hasRock) 
			weaponCarryFrame(i);

		char healthFile[] = "health-r.png";	// need enough chars for NULL terminator!!
		if     (lem.health < 3) healthFile[7] = 'y';
		else  				    healthFile[7] = 'g';

		Coord h = deriveCoord(lem.coord, -4, -10);
		int barWidth = (int)(((double)lem.health / 3) * 8.0);

		drawSpriteFull(makeSimpleSprite("black.png"), deriveCoord(h, -1, -1), 10, 3, 0, false);
		drawSpriteFull(makeSimpleSprite("health-r.png"), h, 8, 1, 0, false);
		drawSpriteFull(makeSimpleSprite(healthFile), h, barWidth, 1, 0, false);
	}
}