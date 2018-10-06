#include "enemy.h"
#include "renderer.h"
#include "assets.h"
#include "time.h"
#include "player.h"
#include "ai.h"
#include "input.h"
#include "scene.h"
#include "fx.h"
#include "lem.h"

static const int ANIM_HZ = 1000 / 4;


// ----------
// Enemies
// ----------
#define WALK_FRAMES 4
long lastIdleTime;
const double MIN_DIR_CHANGE = 500;
const double MAX_DIR_CHANGE = 3000;
const int INITIAL_ENEMIES = 3;
const double ENEMY_SPEED = 1;
const double CHAR_BOUNDS = 15;
const int BLINK_DELAY = 1500;
const int STUN_WAIT = 2000;
const int DEAD_FRAMES = 4;
const int STAR_FRAMES = 4;
long lastDeathFrame;
long lastStarFrame;
Enemy enemies[MAX_ENEMY];


// ----------
// Shots
// ----------
// const double SHOT_SPEED = 1.75;
// //const double SHOT_RELOAD = 1000;
// const int SHOT_FRAMES = 4;
// const int DAMAGE = 25;
// long lastShotFrame;
// Shot shots[MAX_SHOTS];


// ----------
// Explosions
// ----------
typedef struct {
	bool valid;
	Coord coord;
	int animInc;
} Exp;

#define MAX_EXP 20
const double BOUND = 20;
Exp explosions[MAX_EXP];


// ----------
// Puffs
// ----------
typedef struct {
	bool valid;
	Coord coord;
	long spawnTime;
	int animInc;
} Puff;

#define MAX_PUFFS 100
const double PUFF_FREQ = 75;
const double PUFF_DURATION = 750;
const int PUFF_FADE_TIME = 250;
const int PUFF_FRAMES = 3;
long lastExpFrame;
Puff puffs[MAX_PUFFS];


#define MAX_SPAWNS 4
Coord spawns[MAX_SPAWNS];



// ----------
// Helpers
// ----------
char* getColor(int i) {
	char* string = malloc(sizeof(char) * 6);

	switch(enemies[i].color) {
		case 0:
			strcpy(string, "orange");
			break;
		case 1:
			strcpy(string, "green");
			break;
		case 2:
			strcpy(string, "blue");
			break;
		case 3:
			strcpy(string, "red");
			break;
		case 4:
			strcpy(string, "pink");
			break;
	}

	return string;
}

double randomAngle() {
	// Random 8-way direction.

	// return degToRad(chance(50) ? 0 : 180);
	// return degToRad(chance(50) ? 90 : 135);

	double deg;
	switch(randomMq(0, 7)){
		case 1:
			deg = 45;
			break;
		case 2:
			deg = 90;
			break;
		case 3:
			deg = 135;
			break;
		case 4:
			deg = 180;
			break;
		case 5:
			deg = 225;
			break;
		case 6:
			deg = 270;
			break;
		case 7:
			deg = 315;
			break;
		default:
			deg = 0;
			break;
	}

	return degToRad(deg);
}


void spawnExp(Coord c) {
	// find a place to put it in our explosion array.
	for(int i=0; i < MAX_EXP; i++) {
		if(explosions[i].valid) continue;

		// make it
		Exp e = { true, c, 0 };
		explosions[i] = e;

		return;
	}
}

void spawnEnemy(Coord point, int color) {
	// spawn them.
	int lindex = spawnLem(point, color, false);

	// set enemy-specific properties.
	lemmings[lindex].angle = randomAngle();
	lemmings[lindex].en_lastDirTime = clock();
	lemmings[lindex].en_nextDirTime = 500;
}

void respawn(int color) {
	spawnEnemy(
		spawns[randomMq(0,3)],
		color
	);
}

// bool havingBreather(int enemyInc) {
// 	return enemies[enemyInc].lastBreather > 0;
// }

bool wouldTouchEnemy(Coord a, int selfIndex, bool includePlayer) {
	//Check player
	if(includePlayer) {
		if(inBounds(a, makeSquareBounds(pos, CHAR_BOUNDS))) {
			return true;
		}
	}

	//Check enemies.
	for(int i=0; i < MAX_ENEMY; i++) {
		if(selfIndex != i && inBounds(a, makeSquareBounds(enemies[i].coord, CHAR_BOUNDS))) {
			return true;
		}
	}

	return false;
}

void fireAngleShot(int e, double deg) {
	double rad = degToRad(deg);

	// turn enemy TOWARDS where he's shooting
	enemies[e].idleTarget = degToRad(deg);
	enemies[e].lastDirTime = clock();
	enemies[e].nextDirTime = 350;		// quick dir change so we don't collide.

	// find a spare projectile 
	for(int i=0; i < MAX_SHOTS; i++) {
		if(shots[i].valid) continue;

		Coord origin = enemies[e].coord;
		Coord shotStep = getAngleStep(rad, SHOT_SPEED, false);
		Shot s = { true, origin, shotStep, deg, 0, e, 0 };
		shots[i] = s;
		break;
	}

	enemies[e].ammo--;
	enemies[e].lastShot = clock();

	// drop weapon if run out of ammo.
	if(enemies[e].ammo == 0) enemies[e].hasRock = false;
}


// ----------
// Logic
// ----------
void enemyGameFrame(void) {
	for(int i=0; i < MAX_ENEMY; i++) {
		if(!enemies[i].valid) continue;

		// hide corpses after a while.
		if(enemies[i].stunned) {
			if(isDue(clock(), enemies[i].stunnedTime, STUN_WAIT)) {
				enemies[i].valid = false;
				respawn(enemies[i].color);
				continue;
			}
		}

		if(enemies[i].dead) continue;

		// run the AI
		aiSmartFrame(i);
	}

	// home shots towards target.
	for(int i=0; i < MAX_SHOTS; i++) {
		if(!shots[i].valid) continue;

		// move it
		shots[i].coord.x += shots[i].target.x;
		shots[i].coord.y += shots[i].target.y;

		// turn off shots out of range.
		if(!onScreen(shots[i].coord, 0)) 
			shots[i].valid = false;

		// TODO: replace with HitActor function?

		// did we hit an enemy?
		for(int e=0; e < MAX_ENEMY; e++) {
			return;

			if(enemies[e].coord.x == 0) continue;
			if(shots[i].shooter == e) continue;		// don't hit ourselves :p
			if(enemies[e].dead) continue;			// don't hit corpses

			if(inBounds(shots[i].coord, makeSquareBounds(enemies[e].coord, BOUND))) {
				spawnExp(shots[i].coord);
				shots[i].valid = false;
				enemies[e].health -= DAMAGE;
				if(enemies[e].health <=0)
					enemies[e].dead = true;		// make dead
				continue;
			}
		}

		// did we hit the player?
		if(inBounds(shots[i].coord, makeSquareBounds(pos, BOUND))) {
			spawnExp(shots[i].coord);
			shots[i].valid = false;
			// health -= DAMAGE;
			// if(health <=0)
			// 	dead = true;		// make dead
			continue;
		}

		// spawn a puff
		if(timer(&shots[i].lastPuff, PUFF_FREQ)) {
			for(int p=0; p < MAX_PUFFS; p++){
				if(puffs[p].valid) continue;
				Puff puff = { true, shots[i].coord, clock(), 0 };
				puffs[p] = puff;
				break;
			}
		}
	}

	// make puffs go away after a short time.
	for(int p=0; p < MAX_PUFFS; p++){
		if(!puffs[p].valid) continue;
		if(isDue(clock(), puffs[p].spawnTime, PUFF_DURATION)) {
			puffs[p].valid = false;
		}
	}

	// did we pick up a weapon?
	for(int i=0; i < MAX_ENEMY; i++) {
		if(!enemies[i].valid) continue;

		for(int j=0; j < MAX_WEAPONS; j++) {
			if(!weapons[j].valid || weapons[j].pickedUp) continue;

			if(inBounds(enemies[i].coord, makeSquareBounds(weapons[j].coord, 10))) {
				enemies[i].hasRock = true;
				enemies[i].ammo = 5;
				weapons[j].pickedUp = true;
				weapons[j].lastPickup = clock();
			}
		}
	}
}


// ----------
// Animation
// ----------
void enemyFxFrame() {

    if(timer(&lastExpFrame, 1000/12)) {
		// explosions
		for(int i=0; i < MAX_EXP; i++) {
			if(!explosions[i].valid) continue;

			// finish explosion
			if(explosions[i].animInc == 5) {
				explosions[i].valid = false;
				continue;
			}

			explosions[i].animInc++;
		}
	}

    if(timer(&lastStarFrame, 1000/15)) {
		// spinning stars
		for(int i=0; i < MAX_ENEMY; i++) {
			if(!enemies[i].stunned) continue;

			enemies[i].starInc = enemies[i].starInc == STAR_FRAMES-1 ? 0 : enemies[i].starInc + 1;
		}
    }

	// animate deaths (want these faster than walking animation)
	if(timer(&lastDeathFrame, 1000/12)) {
		for(int i=0; i < MAX_ENEMY; i++) {
			if(!enemies[i].dead) continue;
			if(enemies[i].stunned) continue;

			if(enemies[i].deadInc < DEAD_FRAMES-1) {
				enemies[i].deadInc++;
				// NB: stopping is done on render frame, otherwise blip occurs.
			}
		}
	}

	// puffs
	for(int i=0; i < MAX_PUFFS; i++) {
		Puff puff = puffs[i];
		if(!puff.valid) continue;

		// fade out puff anim (note "clever" math at end to do incremental fade)
	    if(puff.animInc < 2 && isDue(clock(), puff.spawnTime, PUFF_FADE_TIME * (puff.animInc+1)))
			puffs[i].animInc++;
	}

	// spin rockets
    if(timer(&lastShotFrame, 1000/12)) {
		for(int i=0; i < MAX_SHOTS; i++) {
			if(!shots[i].valid) continue;

			if(shots[i].animInc < SHOT_FRAMES-1) {
				shots[i].animInc++;
			}else{
				shots[i].animInc = 0;
			}
		}
	}
}

void enemyAnimateFrame(void) {
	//Animate the enemies
	if(timer(&lastIdleTime, ANIM_HZ)) {
		for(int i=0; i < MAX_ENEMY; i++) {
			if(!enemies[i].valid || enemies[i].dead) continue;

			// Standing still
			// GONE

			// Incrementing
			// GONE
		}
	}
}

// ----------
// Rendering
// ----------
void enemyRenderFrame(void){
return;
	// render corpses
	for(int i=0; i < MAX_ENEMY; i++) {
		Enemy en = enemies[i];

		// only do this if we ARE stunned.
		if(!en.valid || !en.dead || !en.stunned) continue;

		char frameFile[25];
		sprintf(frameFile, "lem-%s-stun-02.png", getColor(i));
		SDL_RendererFlip flip = en.corpseDir == 0 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

		// blink out of existence.
		if(isDue(clock(), en.stunnedTime, BLINK_DELAY) && en.starInc % 2) continue;

		// sitting lemming
		drawSprite(makeFlippedSprite(frameFile, flip), en.coord);

		// stars
		sprintf(frameFile, "stars-0%d.png", en.starInc+1);
		drawSprite(makeFlippedSprite(frameFile, flip), en.coord);
	}

	// draw live enemies
	for(int i=0; i < MAX_ENEMY; i++) {
		if(enemies[i].dead) continue;
		if(!enemies[i].valid) continue;

		// apply the animation frame
		char frameFile[25];

		// pick the right color
		sprintf(frameFile, "lem-%s-0%d.png", getColor(i), enemies[i].animInc);

		// are we traveling left or right?
		double deg = radToDeg(enemies[i].idleTarget);
		SDL_RendererFlip flip = deg > 90 && deg < 270 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

		// draw the sprite
		// GONE

		// draw the weapon overlay
		// GONE

		// draw health bar
		char healthFile[] = "health-r.png";	// need enough chars for NULL terminator!!
		if     (enemies[i].health <= 50) healthFile[7] = 'y';
		else  							 healthFile[7] = 'g';

		Coord h = deriveCoord(enemies[i].coord, -4, -12);
		int barWidth = (int)(((double)enemies[i].health / 100.0) * 8.0);
		drawSpriteFull(makeSimpleSprite("black.png"), deriveCoord(h, 0, -1), 10, 3, 0);
		drawSpriteFull(makeSimpleSprite("health-r.png"), h, 8, 1, 0);
		drawSpriteFull(makeSimpleSprite(healthFile), h, barWidth, 1, 0);
	}
}

void enemyDeathRenderFrame() {
	// render dying enemies above the explosion
	for(int i=0; i < MAX_ENEMY; i++) {
		if(!enemies[i].dead || enemies[i].stunned) continue;

		char frameFile[25];
		int animFrame = 0;
		int ypos = 0;

		// bouncing
		switch(enemies[i].deadInc) {
			case 0:
				animFrame = 0;
				ypos = -1;
				break;
			case 1:
				animFrame = 0;
				ypos = -3;
				break;
			case 2:
				animFrame = 0;
				ypos = -1;
				break;
			case 3:
				animFrame = 1;

				// crappy dupe calc for corpse dir.
				double deg = radToDeg(enemies[i].idleTarget);
				enemies[i].corpseDir = deg > 90 && deg < 270 ? 0 : 1;

				enemies[i].stunned = true;
				enemies[i].stunnedTime = clock();
				break;
		}

		sprintf(frameFile, "lem-%s-stun-0%d.png", getColor(i), animFrame+1);
		drawSprite(makeSimpleSprite(frameFile), deriveCoord(enemies[i].coord, 0, ypos));
	}
}

void enemyFxRenderFrame() {
	// draw puffs
	for(int p=0; p < MAX_PUFFS; p++){
		if(!puffs[p].valid) continue;
		char file[12];
		sprintf(file, "puff-0%d.png", puffs[p].animInc+1);
		drawSprite(makeSimpleSprite(file), puffs[p].coord);
	}

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

		// drawSpriteFull(makeSimpleSprite(file), shots[i].coord, 1, shots[i].angle+90);
	}

	// draw explosions
	for(int i=0; i < MAX_EXP; i++) {
		if(!explosions[i].valid) continue;
		char file[10];
		sprintf(file, "exp-0%d.png", explosions[i].animInc+1);
		drawSprite(makeSimpleSprite(file), explosions[i].coord);
	}
}

void initEnemy(void) {
	for(int i=0; i < MAX_SHOTS; i++) 	shots[i].valid = false;
	for(int i=0; i < MAX_ENEMY; i++) 	enemies[i].valid = false;

	spawns[0] = makeCoord(20, 20);
	spawns[1] = makeCoord(300, 220);
	spawns[2] = makeCoord(20, 220);
	spawns[3] = makeCoord(300, 20);

	// Coord point = spawns[randomMq(0, MAX_SPAWNS)];

	// Make the enemies
	for(int i=0; i < INITIAL_ENEMIES; i++) {
		spawnEnemy(
			spawns[i+1],			// hit spawns in sequence, so we don't telefrag.
			randomMq(1, 4)
		);
	}
}