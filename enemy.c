#include "enemy.h"
#include "renderer.h"
#include "assets.h"
#include "time.h"
#include "player.h"
#include "ai.h"
#include "input.h"
#include "scene.h"

static const int ANIM_HZ = 1000 / 4;


// ----------
// Enemies
// ----------
#define WALK_FRAMES 4
long lastIdleTime;
const int INITIAL_ENEMIES = 3;
const double ENEMY_SPEED = 1;
const double CHAR_BOUNDS = 15;
const int CORPSE_WAIT = 1000;
const int DEAD_FRAMES = 4;
const int STAR_FRAMES = 4;
long lastDeathFrame;
long lastStarFrame;
Enemy enemies[MAX_ENEMY];


// ----------
// Shots
// ----------
const double SHOT_SPEED = 1.75;
const double SHOT_RELOAD = 1000;
const int SHOT_FRAMES = 4;
long lastShotFrame;
Shot shots[MAX_SHOTS];


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


// ----------
// Particles
// ----------
typedef struct {
	bool valid;
	Coord coord;
	Coord step;
} Particle;

#define PARTICLE_DENSITY 20
#define MAX_TELE 4
const double PARTICLE_SPEED = 1.2;
const int PARTICLE_TIME = 250;


// ----------
// Teleporters
// ----------
typedef struct {
	bool valid;
	Particle* particles;
	long spawnTime;
} Tele;

#define MAX_SPAWNS 4
Tele teleporters[MAX_TELE];
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

bool onScreen(Coord coord, double threshold) {
	return inBounds(coord, makeRect(
			0 + threshold,
			0 + threshold,
			screenBounds.x - (threshold),
			screenBounds.y - (threshold)
	));
}

double randomEnemyAngle() {
	// Random 8-way direction.

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


void spawnTele(Coord c) {

	for(int i=0; i < MAX_TELE; i++) {
		if(teleporters[i].valid) continue;

		Particle *points = malloc(sizeof(Particle) * PARTICLE_DENSITY);

		// make points
		for(int j=0; j < PARTICLE_DENSITY; j++) {
			// calculate step here since much faster.
			double angle = randomMq(0, 360);
			Coord step = getAngleStep(angle, PARTICLE_SPEED, false);

			Particle p = { true, c, step };
			points[j] = p;
		}

		Tele t = { true, points, clock() };
		teleporters[i] = t;
		break;
	}
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

	spawnTele(point);

	// pop them in in a valid place in the array.
	for(int i=0; i < MAX_ENEMY; i++) {
		if(enemies[i].valid) continue;

		Enemy e = {
			true,
			point,
			randomMq(1, 4),
			0,
			randomEnemyAngle(),
			clock(),
			500,
			0,
			color,
			false,
			0,
			false,
			0,
			0,
			0,
			false
		};

		enemies[i] = e;
		break;
	}
}

void respawn(int color) {
	spawnEnemy(
		spawns[randomMq(0,3)],
		color
	);
}

bool havingBreather(int enemyInc) {
	return enemies[enemyInc].lastBreather > 0;
}

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

bool canShoot(int enemyIndex) {
	return enemies[enemyIndex].hasRock && isDue(clock(), enemies[enemyIndex].lastShot, SHOT_RELOAD);
}

void fireAngleShot(int enemyIndex, double deg) {
	double rad = degToRad(deg);

	// find a spare projectile 
	for(int i=0; i < MAX_SHOTS; i++) {
		if(!shots[i].valid) {
			Coord origin = enemies[enemyIndex].coord;
			Coord shotStep = getAngleStep(rad, SHOT_SPEED, false);
			Shot s = { true, origin, shotStep, deg, 0, enemyIndex, 0 };
			shots[i] = s;
			break;
		}
	}
	enemies[enemyIndex].lastShot = clock();
}


// ----------
// Logic
// ----------
void enemyGameFrame(void) {
	// particles - moving them, and stopping them.
	for(int i=0; i < MAX_TELE; i++) {
		if(!teleporters[i].valid) continue;

		if(isDue(clock(), teleporters[i].spawnTime, PARTICLE_TIME)) {
			teleporters[i].valid = false;
			continue;
		}

		for(int j=0; j < PARTICLE_DENSITY; j++) {
			Particle p = teleporters[i].particles[j];
			teleporters[i].particles[j].coord = deriveCoord(p.coord, p.step.x, p.step.y);
		}
	}

	for(int i=0; i < MAX_ENEMY; i++) {
		if(!enemies[i].valid) continue;

		// hide corpses after a while.
		if(enemies[i].buried) {
			if(isDue(clock(), enemies[i].buriedTime, CORPSE_WAIT)) {
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
			if(enemies[e].coord.x == 0) continue;
			if(shots[i].shooter == e) continue;		// don't hit ourselves :p
			if(enemies[e].dead) continue;			// don't hit corpses

			if(inBounds(shots[i].coord, makeSquareBounds(enemies[e].coord, BOUND))) {
				spawnExp(shots[i].coord);
				shots[i].valid = false;
				enemies[e].dead = true;		// make dead
				continue;
			}
		}

		// did we hit the player?
		if(inBounds(shots[i].coord, makeSquareBounds(pos, BOUND))) {
			spawnExp(shots[i].coord);
			shots[i].valid = false;
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
			if(!enemies[i].buried) continue;

			enemies[i].starInc = enemies[i].starInc == STAR_FRAMES-1 ? 0 : enemies[i].starInc + 1;
		}
    }

	// animate deaths (want these faster than walking animation)
	if(timer(&lastDeathFrame, 1000/12)) {
		for(int i=0; i < MAX_ENEMY; i++) {
			if(!enemies[i].dead) continue;
			if(enemies[i].buried) continue;

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
	if(!timer(&lastIdleTime, ANIM_HZ)) return;

	//Animate the enemies
	for(int i=0; i < MAX_ENEMY; i++) {
		if(!enemies[i].valid) continue;
		if(enemies[i].dead) continue;

		// tell lemmings to stand still when taking a breather.
		if(havingBreather(i)) {
			enemies[i].animInc = 2;
			continue;
		}

		//Slight hack - we want to move the enemies in sync with their animation.

		//Increment animations.
		if(enemies[i].animInc < 4) {
			enemies[i].animInc++;
		}else{
			enemies[i].animInc = 1;
		}
	}
}

// ----------
// Rendering
// ----------
void enemyRenderFrame(void){

	// render corpses
	for(int i=0; i < MAX_ENEMY; i++) {
		// if we're not buried.
		if(!enemies[i].valid || !enemies[i].dead || !enemies[i].buried) continue;

		char frameFile[25];
		sprintf(frameFile, "lem-%s-stun-02.png", getColor(i));
		SDL_RendererFlip flip = enemies[i].corpseDir == 0 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

		// sitting lemming
		drawSprite(makeFlippedSprite(frameFile, flip), enemies[i].coord);

		// stars
		sprintf(frameFile, "stars-0%d.png", enemies[i].starInc+1);
		drawSprite(makeFlippedSprite(frameFile, flip), enemies[i].coord);
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
		Sprite sprite = makeFlippedSprite(frameFile, flip);
		drawSprite(sprite, enemies[i].coord);

		// draw weapon overlay
		if(enemies[i].hasRock) {
			int yoff = enemies[i].animInc % 2 ? 0 : -2;
			drawSprite(makeFlippedSprite("w_rock-0.png", flip), deriveCoord(enemies[i].coord, flip ? -3 : 3, 1+yoff));

			// WIP: Rotating weapon sprite based on where enemy is walking (could easily change this to switch)
//			drawSpriteFull(makeSimpleSprite("w_rock-0.png"), deriveCoord(enemies[i].coord, flip ? -3 : 3, 1+yoff), 1, radToDeg(enemies[i].idleTarget) - 180);
		}
	}
}

void enemyDeathRenderFrame() {
	// render dying enemies above the explosion
	for(int i=0; i < MAX_ENEMY; i++) {
		if(!enemies[i].dead || enemies[i].buried) continue;

		char frameFile[25];
		SDL_RendererFlip flip = SDL_FLIP_NONE;
		int animFrame = 0;
		int ypos = 0;

		// flip second frame for "animation"
		switch(enemies[i].deadInc) {
			case 0:
				// flip = SDL_FLIP_HORIZONTAL;
				animFrame = 0;
				ypos = -1;
				break;
			case 1:
				animFrame = 0;
				ypos = -3;
				break;
			case 2:
				// flip = SDL_FLIP_HORIZONTAL;
				animFrame = 0;
				ypos = -1;
				break;
			case 3:
				animFrame = 1;
				enemies[i].corpseDir = randomMq(0,1);	// left or right
				enemies[i].buried = true;
				enemies[i].buriedTime = clock();
				break;
		}

		sprintf(frameFile, "lem-%s-stun-0%d.png", getColor(i), animFrame+1);

		Sprite sprite = makeFlippedSprite(frameFile, flip);
		drawSprite(sprite, deriveCoord(enemies[i].coord, 0, ypos));
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

	// teleportations
	for(int i=0; i < MAX_TELE; i++) {
		if(!teleporters[i].valid) continue;

		for(int j=0; j < PARTICLE_DENSITY; j++) {
			drawSprite(makeSimpleSprite("tele-0.png"), teleporters[i].particles[j].coord);
		}
	}
}

void initEnemy(void) {
	for(int i=0; i < MAX_SHOTS; i++) 	shots[i].valid = false;
	for(int i=0; i < MAX_ENEMY; i++) 	enemies[i].valid = false;
	for(int i=0; i < MAX_TELE; i++) 	teleporters[i].valid = false;

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