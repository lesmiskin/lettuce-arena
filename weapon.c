#include <time.h>
#include "lem.h"
#include "assets.h"
#include "weapon.h"
#include "renderer.h"
#include "fx.h"
#include "hud.h"
#include "state.h"

const double SHOT_SPEED = 1.75;
const double SHOT_DIST = 13;//13;
const int SHOT_FRAMES = 4;
const int DAMAGE = 25;
const double SHOT_RELOAD = 500;
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
				lemmings[e].health -= DAMAGE;

				// if hit lemming is dead.
				if(lemmings[e].health <=0) {
					lemmings[shots[i].shooter].frags++;
					lemmings[e].dead = true;		// make dead
					lemmings[e].deadTime = clock();
					lemmings[e].active = false;
					lemmings[e].killer = shots[i].shooter;
					lemmings[e].hasRock = false;

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
				}else{
					spawnExp(shots[i].coord, true);
				}
				return;
			}
		}

		// spawn a puff
		if(timer(&shots[i].lastPuff, PUFF_FREQ)) {
			spawnPuff(shots[i].coord);
		}
	}
}

void weaponRenderFrame() {
	// draw shots 
	for(int i=0; i < MAX_SHOTS; i++) {
		if(!shots[i].valid) continue;
		char file[15];

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
			case 305:
				sprintf(file, "rocket-nw.png");
				drawSprite(makeSimpleSprite(file), shots[i].coord);
				break;
		}
	}
}

void shoot(int i, double deg) {
	// only shoot if we have ammo.
	if(!lemmings[i].active || lemmings[i].ammo == 0 || !isDue(clock(), lemmings[i].lastShot, SHOT_RELOAD)) return;

	double rad = degToRad(deg);

	// turn enemy TOWARDS where he's shooting
	if(lemmings[i].isEnemy){
		lemmings[i].angle = degToRad(deg);
		lemmings[i].en_lastDirTime = clock();
		lemmings[i].en_nextDirTime = 30;		// quick dir change so we don't collide.
	}

	// find a spare projectile 
	for(int j=0; j < MAX_SHOTS; j++) {
		if(shots[j].valid) continue;

		Coord origin = extendOnAngle(lemmings[i].coord, lemmings[i].angle, SHOT_DIST);
		Coord shotStep = getAngleStep(rad, SHOT_SPEED, false);
		Shot s = { true, origin, shotStep, deg, 0, i, 0 };
		shots[j] = s;
		break;
	}

	lemmings[i].ammo--;
	lemmings[i].lastShot = clock();

	// drop weapon if run out of ammo.
	if(lemmings[i].ammo == 0) lemmings[i].hasRock = false;
}
