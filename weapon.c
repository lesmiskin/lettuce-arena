#include <time.h>
#include "lem.h"
#include "assets.h"
#include "weapon.h"
#include "renderer.h"
#include "fx.h"
#include "hud.h"
#include "state.h"

// ROCKET LAUNCHER
const int W_ROCK = 2;
const double ROCK_SPEED = 1.75;
const double ROCK_RELOAD = 750;
const double ROCK_DAMAGE = 34;
const double ROCK_PUSH = 3;

// MACHINE GUN
const int W_MACH = 1;
const double MACH_SPEED = 2.5;
const double MACH_RELOAD = 150;
const double MACH_DAMAGE = 5;
const double MACH_PUSH = 0.5;

const double SHOT_DIST = 13;//13;
const int SHOT_FRAMES = 4;
const int DAMAGE = 1;
const int PUSH_AMOUNT = 4;
long lastShotFrame;
long lastPlayerKillTime;
int lastPlayerKillIndex = -1;

Shot shots[MAX_SHOTS];

void initWeapon() {
	for(int i=0; i < MAX_SHOTS; i++) 
		shots[i].valid = false;
}

bool onScreen(Coord coord, double threshold) {
	return inBounds(coord, makeRect(
			0 + threshold,
			0 + threshold,
			screenBounds.x - (threshold),
			screenBounds.y - (threshold)
	));
}

void weaponGameFrame() {
	for(int i=0; i < MAX_SHOTS; i++) {
		if(!shots[i].valid) continue;

		// Move the shot.
		shots[i].coord.x += shots[i].target.x;
		shots[i].coord.y += shots[i].target.y;

		// Turn off shot if outside window.
		if(!onScreen(shots[i].coord, 0)) 
			shots[i].valid = false;

		// Did we hit a lemming?
		for(int e=0; e < MAX_LEM; e++) {
			if(!lemmings[e].valid) continue;
			if(shots[i].shooter == e) continue;		// don't hit ourselves :p
			if(lemmings[e].dead) continue;			// don't hit corpses

			if(inBounds(shots[i].coord, makeSquareBounds(lemmings[e].coord, LEM_BOUND))) {
				shots[i].valid = false;
				if(lemmings[e].isPlayer && CHEAT_GOD) continue;

				lemmings[e].health -= shots[i].type == W_ROCK ? ROCK_DAMAGE : MACH_DAMAGE;
				lemmings[e].lastHit = clock();

				// if hit lemming is dead.
				if(lemmings[e].health <=0) {
					lemmings[shots[i].shooter].frags++;
					lemmings[e].dead = true;		// make dead
					lemmings[e].deadTime = clock();
					lemmings[e].active = false;
					lemmings[e].killer = shots[i].shooter;
					lemmings[e].weap = 0;

					spawnExp(shots[i].coord, false);
					spawnLemExp(shots[i].coord, lemmings[e].color);

					if(lemmings[shots[i].shooter].isPlayer) {
						lastPlayerKillTime = clock();
						lastPlayerKillIndex = e;
					}

					// hit the fraglimit
					if(lemmings[shots[i].shooter].frags >= fraglimit) {
						for(int j=0; j < MAX_LEM; j++) {
							lemmings[j].active = false;
							spawnTele(lemmings[j].coord);	// flash everyone out of existence.
						}
						gameOver();
					}
				// small explosion if we just hit them, but didn't kill 'em
				}else{
					if(shots[i].type == W_ROCK)
						spawnExp(shots[i].coord, false);

					spawnHurt(lemmings[e].coord, lemmings[e].color);

					// push them back.
//					double blastAngle = getAngle(shots[i].coord, lemmings[e].coord);
					double blastAngle = degToRad(shots[i].angle);
					lemmings[e].pushAmount = shots[i].type == W_ROCK ? ROCK_PUSH : MACH_PUSH;
					lemmings[e].pushAngle = blastAngle;
				}
				return;
			}
		}

		// spawn a puff
		if(shots[i].type == W_ROCK && timer(&shots[i].lastPuff, PUFF_FREQ)) {
			spawnPuff(shots[i].coord);
		}
	}
}

void weaponRenderFrame() {
	// draw shots 
	for(int i=0; i < MAX_SHOTS; i++) {
		if(!shots[i].valid) continue;
		char file[15];

		if(shots[i].type == W_MACH) {
			drawSprite(makeSimpleSprite("bullet.png"), shots[i].coord);
			continue;
		}

		switch((int)shots[i].angle+90) {
			case 360:
				sprintf(file, "rocket-n.png");
				drawSprite(makeSimpleSprite(file), shots[i].coord);
				break;
			case 90:
				sprintf(file, "rocket-e.png");
				drawSprite(makeSimpleSprite(file), shots[i].coord);
				break;
			case 180:
				sprintf(file, "rocket-s.png");
				drawSprite(makeSimpleSprite(file), shots[i].coord);
				break;
			case 270:
				sprintf(file, "rocket-w.png");
				drawSprite(makeSimpleSprite(file), shots[i].coord);
				break;

			// TODO: these angles are kind of weird. figure out why this is :p
			case 405:
				sprintf(file, "rocket-ne.png");
				drawSprite(makeSimpleSprite(file), shots[i].coord);
				break;
			case 135:
				sprintf(file, "rocket-se.png");
				drawSprite(makeSimpleSprite(file), shots[i].coord);
				break;
			case 225:
				sprintf(file, "rocket-sw.png");
				drawSprite(makeSimpleSprite(file), shots[i].coord);
				break;
			case 315:
			default:	// :p (hack)
				sprintf(file, "rocket-nw.png");
				drawSprite(makeSimpleSprite(file), shots[i].coord);
				break;
		}
	}
}

void shootMach(int i, double deg) {
	double rad = degToRad(deg);

	// turn enemy TOWARDS where he's shooting
	if(lemmings[i].isEnemy){
		lemmings[i].angle = rad;
		lemmings[i].en_lastDirTime = clock();
		lemmings[i].en_nextDirTime = 30;		// quick dir change so we don't collide.
	}

	// find a spare projectile 
	for(int j=0; j < MAX_SHOTS; j++) {
		if(shots[j].valid) continue;

		Coord origin = extendOnAngle(lemmings[i].coord, lemmings[i].angle, SHOT_DIST);
		Coord shotStep = getAngleStep(rad, MACH_SPEED, false);
		Shot s = { true, origin, shotStep, deg, 0, i, 0, W_MACH };
		shots[j] = s;
		break;
	}
}

void shootRock(int i, double deg) {
	double rad = degToRad(deg);

	// turn enemy TOWARDS where he's shooting
	if(lemmings[i].isEnemy){
		lemmings[i].angle = rad;
		lemmings[i].en_lastDirTime = clock();
		lemmings[i].en_nextDirTime = 30;		// quick dir change so we don't collide.
	}

	// find a spare projectile 
	for(int j=0; j < MAX_SHOTS; j++) {
		if(shots[j].valid) continue;

		Coord origin = extendOnAngle(lemmings[i].coord, lemmings[i].angle, SHOT_DIST);
		Coord shotStep = getAngleStep(rad, ROCK_SPEED, false);
		Shot s = { true, origin, shotStep, deg, 0, i, 0, W_ROCK };
		shots[j] = s;
		break;
	}
}

bool isReloading(int i) {
	return !isDue(clock(), lemmings[i].lastShot, lemmings[i].weap == W_ROCK ? ROCK_RELOAD : MACH_RELOAD);
}

void shoot(int i, double deg) {
	// only shoot if we have ammo.
	if(!lemmings[i].active || lemmings[i].ammo == 0 || isReloading(i)) return;

	// shoot relevant gun
	if(lemmings[i].weap == W_ROCK) shootRock(i, deg);
	if(lemmings[i].weap == W_MACH) shootMach(i, deg);

	// reduce ammo etc.
	lemmings[i].ammo--;
	lemmings[i].lastShot = clock();

	// drop weapon if run out of ammo.
	if(lemmings[i].ammo == 0) lemmings[i].weap = 0;
}
