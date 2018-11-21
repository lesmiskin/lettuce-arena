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

int xTween = 0;

const int PAIN_DURATION = 200;
const int RESPAWN_TIME = 2000;
const int LEM_HEALTH = 100;
const int BAR_WIDTH = 8;

static int MUZZLE_DIST = 7;

Lem lemmings[MAX_LEM];
const int PLAYER_INDEX = 0;
static long lastIdleTime;
static const int ANIM_HZ = 1000 / 4;
const double LEM_SPEED = 1;
const double LEM_BOUND = 17;
const double BORDER_X = 5;
const double BORDER_Y = 10;
const int WEAP_BOUND = 16;
const double PUSH_SPEED = 1.035;

static int arrowXOrigin;
static int arrowYOrigin;
static Coord arrowX;
static Coord arrowY;

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

	if(!isPlayer) {
		// otherwise find valid insert position in main lemming array.
		for(int i=1; i < MAX_LEM; i++) {
			if(lemmings[i].valid) continue;
			insertIndex	= i;
			break;
		}

	}

	// reset player marker (set at opposing ends so we can see it come in - whole idea!)
	if(isPlayer){
		arrowXOrigin = coord.x < screenBounds.x/2 ? screenBounds.x : 0;
		arrowYOrigin = coord.y < screenBounds.y/2 ? screenBounds.y : 0;
	}

	// Set up the LEM object with all his properties.
	Lem l = {
		-1,
		name,
		frags,
		clock(),
		0,
		clock(),
		SDL_GetTicks(),
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
		-1,				// lastHIt
		0,
		zeroCoord(0,0),	// pickup coord
		"",

		// weapons
		0,
		false,
		false,
		0,
		0,
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

	l.lastItem = malloc(sizeof(char) * 15);
	strcpy(l.lastItem, "");

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

bool inPain(int i) {
	return lemmings[i].lastHit > 0 && !isDue(clock(), lemmings[i].lastHit, PAIN_DURATION);
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
			Weapon w = weapons[j];

			if(inBounds(lemmings[i].coord, makeSquareBounds(weapons[j].coord, WEAP_BOUND))) {
				char name[15];

				// NB: Powerups we invalidate immediately, whereas respawning weapons we just
				// temporarily disable. This lets powerups spawn *anywhere*

				// pickup weap
				if(w.type == W_ROCK || w.type == W_MACH) {
					lemmings[i].weap = w.type;
					lemmings[i].lastWeap = w.type;
					lemmings[i].ammo = getMaxAmmo(w.type);
					weapons[j].pickedUp = true;
					weapons[j].lastPickup = clock();
					if(w.type == W_ROCK) 
						sprintf(name, "rocket launcher");
					else
						sprintf(name, "machine gun");
				// health
				} else if(w.type == I_HEALTH) {
					lemmings[i].health = LEM_HEALTH * 2;
					sprintf(name, "very health");
					weapons[j].valid = false;
					powerupPickup();
				// ammo
				} else if(w.type == I_AMMO) {
					lemmings[i].ammo = getMaxAmmo(lemmings[i].weap) * 2;
					sprintf(name, "very ammo");
					weapons[j].valid = false;
					powerupPickup();
				}

				lemmings[i].lastPickup = clock();
				lemmings[i].pickupCoord = w.coord;
				strcpy(lemmings[i].lastItem, name);
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
	Coord fireDerive = zeroCoord();

	// weapon rotation and position.
	switch((int)radToDeg(lemmings[i].angle)+90) {
		case 360:
			sprintf(file, "w_rock-n.png");
			derive = makeCoord(3, -4);
			fireDerive = makeCoord(0, 1);
			break;
		case 90:
			sprintf(file, "w_rock-e.png");
			derive = makeCoord(3, 1);
			fireDerive = makeCoord(-1, 0);
			break;
		case 270:
			sprintf(file, "w_rock-w.png");
			derive = makeCoord(-3, 1);
			fireDerive = makeCoord(1, 0);
			break;
		case 180:
			sprintf(file, "w_rock-s.png");
			derive = makeCoord(-2, 5);
			fireDerive = makeCoord(0, -1);
			break;

		case 405://ne
			sprintf(file, "w_rock-ne.png");
			derive = makeCoord(4, -3);
			fireDerive = makeCoord(-1, 1);
			break;
		case 135://se
			sprintf(file, "w_rock-se.png");
			derive = makeCoord(2, 3);
			fireDerive = makeCoord(-1, -1);
			break;
		case 225://sw
			sprintf(file, "w_rock-sw.png");
			derive = makeCoord(-2, 3);
			fireDerive = makeCoord(1, -1);
			break;
		default:
		case 315://nw
			sprintf(file, "w_rock-nw.png");
			derive = makeCoord(-4, -3);
			fireDerive = makeCoord(1, 1);
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
		case 315://nw
		default:
			if(lemmings[i].animInc == 1) { xoff = -1; yoff = 2; }	// backstep
			if(lemmings[i].animInc == 2) { xoff = 0;  yoff = 0; }	// up
			if(lemmings[i].animInc == 3) { xoff = 1;  yoff = 2; }	// forwardstep
			if(lemmings[i].animInc == 4) { xoff = 0;  yoff = 0; }	// up
			break;
	}

	// Establish weapon origin.
	Coord wc = deriveCoord(lemmings[i].coord, derive.x, derive.y);
	wc = deriveCoord(wc, xoff, yoff);

	const double MACH_SCALE = 0.5;
	const double ROCK_SCALE = 0.9;

	int recoil = 0;

	// muzzle flash
	if(lemmings[i].lastWeap == W_MACH) {
		if(!isDue(clock(), lemmings[i].lastShot, 25)) {
			Coord muzzPos = extendOnAngle(wc, lemmings[i].angle, MUZZLE_DIST);
			drawSpriteFull(makeSimpleSprite("exp-01.png"), muzzPos, MACH_SCALE, MACH_SCALE, chance(50) ? 0 : 180, true);
			recoil = 2;
		}
		else if(!isDue(clock(), lemmings[i].lastShot, 25*2)) {
			Coord muzzPos = extendOnAngle(wc, lemmings[i].angle, MUZZLE_DIST);
			drawSpriteFull(makeSimpleSprite("exp-01.png"), muzzPos, MACH_SCALE, MACH_SCALE, chance(50) ? 45 : 45*3, true);
			recoil = 1;
		}
	}else if(lemmings[i].lastWeap == W_ROCK) {
		if(!isDue(clock(), lemmings[i].lastShot, 100)) {
			Coord muzzPos = extendOnAngle(wc, lemmings[i].angle, MUZZLE_DIST);
			drawSpriteFull(makeSimpleSprite("exp-04.png"), muzzPos, ROCK_SCALE, ROCK_SCALE, radToDeg(randomAngle()), true);
			recoil = 6;
		}
		else if(!isDue(clock(), lemmings[i].lastShot, 150)) {
			Coord muzzPos = extendOnAngle(wc, lemmings[i].angle, MUZZLE_DIST);
			drawSpriteFull(makeSimpleSprite("exp-05.png"), muzzPos, ROCK_SCALE, ROCK_SCALE, radToDeg(randomAngle()), true);
			recoil = 3;
		}
		else if(!isDue(clock(), lemmings[i].lastShot, 200)) {
			Coord muzzPos = extendOnAngle(wc, lemmings[i].angle, MUZZLE_DIST);
			drawSpriteFull(makeSimpleSprite("exp-06.png"), muzzPos, ROCK_SCALE, ROCK_SCALE, radToDeg(randomAngle()), true);
			recoil = 2;
		}
		else if(!isDue(clock(), lemmings[i].lastShot, 250)) {
			Coord muzzPos = extendOnAngle(wc, lemmings[i].angle, MUZZLE_DIST);
			drawSpriteFull(makeSimpleSprite("exp-06.png"), muzzPos, ROCK_SCALE, ROCK_SCALE, radToDeg(randomAngle()), true);
			recoil = 1;
		}
	}

	// weapon recoil
	if(recoil > 0)
		wc = deriveCoord(wc, fireDerive.x * recoil, fireDerive.y * recoil);

	// draw weapon (yes, weapon should flash too :p)
	AssetVersion spriteVersion = inPain(i) ? ASSET_WHITE : ASSET_DEFAULT;
	Sprite lemSprite = makeSprite(getTextureVersion(file, spriteVersion), zeroCoord(), SDL_FLIP_NONE);
	drawSprite(lemSprite, wc);
}

int getReloadTime(int i) {
	if(lemmings[i].weap == W_ROCK ? (int)ROCK_RELOAD : (int)MACH_RELOAD);
}

bool canShoot(int i) {
	Lem l = lemmings[i];
	return l.ammo > 0 && l.weap > 0 && isDue(clock(), l.lastShot, getReloadTime(i));
}

float quadEaseOut (float t, float b, float c, float d) {
	// quadratic ease out (http://gizma.com/easing)
	t /= d;
	return -c * t*(t-2) + b;	
};

float expoEaseOut (float t, float b, float c, float d) {
	// if(b < c) {
	// 	return b - (c * (-pow( 2, -10 * t/d ) + 1 ));
	// }
	return c * (-pow( 2, -10 * t/d ) + 1 ) + b;
};

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
		// spawn "you are here" signal upon respawn.
		if(lem.isPlayer && !isDue(clock(), lem.spawnTime, PRACTICE_WAIT) && PLAYER_INDICATOR) {

			// reset on spawn.
			// plunge in opposite dir's.
			// stop when already near the playa.

			Coord target = lem.coord;

			arrowX.y = target.y;
			arrowX.x = expoEaseOut(SDL_GetTicks() - lem.spawnTimeSdl, 0, target.x, PRACTICE_WAIT*2.5);
			// if(lem.coord.x < screenBounds.x/2) arrowX.x = screenBounds.x - arrowX.x;
			// drawSpriteFull(makeSimpleSprite("arrow.png"), arrowX, 1, 1, 270, true);

			arrowY.x = target.x;
			arrowY.y = expoEaseOut(SDL_GetTicks() - lem.spawnTimeSdl, 0, target.y, PRACTICE_WAIT*2.5);
			// if(lem.coord.y < screenBounds.y/2) arrowY.y = arrowY.y - screenBounds.y;
			// drawSprite(makeSimpleSprite("arrow.png"), arrowY);

			if(isDue(clock(), lem.lastFlash, 100)) {
				lemmings[i].flashInc = !lemmings[i].flashInc;
				lemmings[i].lastFlash = clock();
			}
			if(lem.flashInc) {
				drawSprite(makeSimpleSprite("flash.png"), lem.coord);
				writeFontFull(lem.name, deriveCoord(lem.coord, 0, -18), false, true);
			}
		}

		// names
		if(lemmings[PLAYER_INDEX].dead || checkCommand(CMD_SCORES) || (showName && !isDue(clock(), lem.spawnTime, PRACTICE_WAIT))) {
			writeFontFull(lem.name, deriveCoord(lem.coord, 0, -18), false, true);
		}

		// draw the sprite
		AssetVersion spriteVersion = inPain(i) ? ASSET_WHITE : ASSET_DEFAULT;
		Sprite lemSprite = makeSprite(getTextureVersion(frameFile, spriteVersion), zeroCoord(), flip);
		drawSprite(lemSprite, lem.coord);

		// draw carrying weapon
		if(lem.weap > 0 || !isDue(clock(), lem.lastShot, getReloadTime(i)) )
			weaponCarryFrame(i);

		// show nearby ammo counter if low
		Lem p = lemmings[PLAYER_INDEX];
		if(p.weap > 0 && p.ammo <= getMaxAmmo(p.weap)/4) {
			// show temporary counter next to player when firing
			if(!isDue(clock(), p.lastShot, 500)) {
				writeAmount(p.ammo, deriveCoord(lemmings[PLAYER_INDEX].coord, 7, 7));
			}
		}

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
		if(barWidth < 1) barWidth = 1;	// always show something (otherwise invisible)!

		drawSpriteFull(makeSimpleSprite("black.png"), deriveCoord(h, 1, 1), barWidth+1, 3, 0, false);
		drawSpriteFull(makeSimpleSprite(healthFile), h, barWidth, 1, 0, false);
	}
}