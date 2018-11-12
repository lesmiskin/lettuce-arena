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
#include "input.h"

Coord spawns[MAX_SPAWNS];

const int RESPAWN_TIME = 2000;
const int LEM_HEALTH = 3;
const int BAR_WIDTH = 8;

Lem lemmings[MAX_LEM];
const int PLAYER_INDEX = 0;
static long lastIdleTime;
static const int ANIM_HZ = 1000 / 4;
const double LEM_SPEED = 1;
const double LEM_BOUND = 17;
const double BORDER_X = 5;
const double BORDER_Y = 10;
const int WEAP_BOUND = 16;
const double PUSH_SPEED = 1.1;

Dir4 makeFreeDir() {
	Dir4 d = { true, true, true, true };
	return d;
}

Move makeMove(Coord final, Dir4 dir) {
	Move m = { final, dir };
	return m;
}

Move tryMove(Coord target, Coord origin, int selfIndex) {
	Coord permitted = target;
	Dir4 allowDir = makeFreeDir();
	Dir4 intendDir = { 
		target.y < origin.y, target.y > origin.y,
		target.x < origin.x, target.x > origin.x,
	};

	// --------------------
	// block screen borders
	// --------------------
	if(target.x <= BORDER_X) 
		allowDir.left = false;
	if(target.x > screenBounds.x-BORDER_X)
		allowDir.right = false;
	if(target.y <= BORDER_Y)
		allowDir.up = false;
	if(target.y > screenBounds.y-BORDER_Y)
		allowDir.down = false;

	// ------------------
	// block other actors
	// ------------------
	for(int i=0; i < MAX_LEM; i++) {
		if(i == selfIndex || !lemmings[i].active) continue;

		int halfBound = LEM_BOUND/2;
		if(inBounds(target, makeSquareBounds(lemmings[i].coord, LEM_BOUND))) {
			// we've detected an obstruction. let's SEND BACK a coordinate that represents
			// the LIMITED movement based on WHERE its obstructing.

			// is the X AXIS free?
			Coord xTry = makeCoord(target.x, origin.y);
			for(int j=0; j < MAX_LEM; j++) {
				if(j == selfIndex) continue;
				// as soon as we encounter a visible obstruction - STOP ON THIS AXIS.
				if(inBounds(xTry, makeSquareBounds(lemmings[j].coord, LEM_BOUND))) {
					if(intendDir.right) allowDir.right = false;
					if(intendDir.left) allowDir.left = false;
					break;
				}
			}

			// is the Y AXIS free?
			Coord yTry = makeCoord(origin.x, target.y);
			for(int j=0; j < MAX_LEM; j++) {
				if(j == selfIndex) continue;
				// as soon as we encounter a visible obstruction - STOP ON THIS AXIS.
				if(inBounds(yTry, makeSquareBounds(lemmings[j].coord, LEM_BOUND))) {
					if(intendDir.up) allowDir.up = false;
					if(intendDir.down) allowDir.down = false;
					break;
				}
			}
		}
	}

	// limit resultant coord based on movement ability
	if(!allowDir.left || !allowDir.right) permitted.x = origin.x;
	if(!allowDir.up || !allowDir.down) permitted.y = origin.y;

	return makeMove(permitted, allowDir);
}

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
		LEM_HEALTH,		// health
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

			if(inBounds(lemmings[i].coord, makeSquareBounds(weapons[j].coord, WEAP_BOUND))) {
				lemmings[i].hasRock = true;
				lemmings[i].ammo += 3;
				weapons[j].pickedUp = true;
				weapons[j].lastPickup = clock();
			}
		}

		// blast pushing
		if(lemmings[i].pushAmount > 0.1) {
			Coord step = getAngleStep(lemmings[i].pushAngle, lemmings[i].pushAmount, false);
			Coord target = deriveCoord(lemmings[i].coord, step.x, step.y);
			lemmings[i].pushAmount /= PUSH_SPEED;
			lemmings[i].coord = tryMove(target, lemmings[i].coord, i).result;
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

		// player plume
		if(lem.isPlayer){
			// spawn "you are here" signal upon respawn.
			if(!isDue(clock(), lem.spawnTime, PRACTICE_WAIT)) {
				if(isDue(clock(), lem.lastFlash, 100)) {
					lemmings[i].flashInc = !lemmings[i].flashInc;
					lemmings[i].lastFlash = clock();
				}
				if(lem.flashInc) {
					drawSprite(makeSimpleSprite("flash.png"), lem.coord);
					writeFontFull(lem.name, deriveCoord(lem.coord, 0, -18), false, true);
				}
			} else {
				showName = true;
			}
		}

		// names
		if(lemmings[PLAYER_INDEX].dead || checkCommand(CMD_SCORES) || (showName && !isDue(clock(), lem.spawnTime, PRACTICE_WAIT))) {
			writeFontFull(lem.name, deriveCoord(lem.coord, 0, -18), false, true);
		}

		// draw the sprite
		Sprite lemSprite = makeFlippedSprite(frameFile, flip);
		drawSprite(lemSprite, lem.coord);

		// draw carrying weapon
		if(lem.hasRock) 
			weaponCarryFrame(i);


		// -----------
		// health bars
		// -----------
		char healthFile[] = "health-r.png";	// need enough chars for NULL terminator!!

		// show as green if 2 or above.
		if(lem.health >= (double)LEM_HEALTH / 2) {
			healthFile[7] = 'g';
		}

		Coord h = deriveCoord(lem.coord, -4, -10);
		int barWidth = (int)(((double)lem.health / LEM_HEALTH) * BAR_WIDTH);

		drawSpriteFull(makeSimpleSprite("black.png"), deriveCoord(h, -1, -1), barWidth+1, 3, 0, false);
		drawSpriteFull(makeSimpleSprite(healthFile), h, barWidth, 1, 0, false);
	}
}