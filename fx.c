#include "fx.h"
#include "lem.h"
#include "assets.h"
#include "time.h"
#include "scene.h"
#include "renderer.h"

// ----------
// Explosions
// ----------
typedef struct {
	bool valid;
	Coord coord;
	int animInc;
	bool smallExp;
} Exp;

#define MAX_EXP 20
Exp explosions[MAX_EXP];
long lastExpFrame;


// ----------
// Puffs
// ----------
typedef struct {
	bool valid;
	Coord coord;
	long spawnTime;
	int animInc;
} Puff;

const double PUFF_FREQ = 75;
const double PUFF_DURATION = 500;
const int PUFF_FADE_TIME = 100;
const int PUFF_FRAMES = 3;
long lastExpFrame;
Puff puffs[MAX_PUFFS];
bool enablePuffs = false;


const double BOUND = 20;


typedef struct {
	bool valid;
	Coord coord;
	Coord step;
	int color;
	int lemColor;
} Particle;

typedef struct {
    bool valid;
    Particle* particles;
    long spawnTime;
	int duration;
	bool isLemExp;
} Tele;

#define PARTICLE_DENSITY 20
#define MAX_TELE 4
const double PARTICLE_SPEED = 1.2;


Tele teleporters[MAX_TELE];

void fxGameFrame() {
	// puffs
	for(int i=0; i < MAX_PUFFS; i++) {
		Puff puff = puffs[i];
		if(!puff.valid) continue;

		// make puffs go away after a short time.
		if(isDue(clock(), puffs[i].spawnTime, PUFF_DURATION)) {
			puffs[i].valid = false;
			continue;
		}

		// fade out puff anim (note "clever" math at end to do incremental fade)
	    if(puff.animInc < 2 && isDue(clock(), puff.spawnTime, PUFF_FADE_TIME * (puff.animInc+1)))
			puffs[i].animInc++;
	}

	// explosions
    if(timer(&lastExpFrame, 1000/12)) {
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

	// particles - moving them, and stopping them.
	for(int i=0; i < MAX_TELE; i++) {
		if(!teleporters[i].valid) continue;

		if(isDue(clock(), teleporters[i].spawnTime, teleporters[i].duration)) {
			teleporters[i].valid = false;
			continue;
		}

		for(int j=0; j < PARTICLE_DENSITY; j++) {
			Particle p = teleporters[i].particles[j];
			teleporters[i].particles[j].coord = deriveCoord(p.coord, p.step.x, p.step.y);
		}
	}
}

void fxRenderFrame() {
	// draw puffs
	for(int p=0; p < MAX_PUFFS; p++){
		if(!puffs[p].valid) continue;
		char file[12];
		sprintf(file, "puff-0%d.png", puffs[p].animInc+1);
		drawSprite(makeSimpleSprite(file), puffs[p].coord);
	}

	// draw explosions
	for(int i=0; i < MAX_EXP; i++) {
		if(!explosions[i].valid) continue;
		char file[10];
		double scale = explosions[i].smallExp ? 0.5 : 1;
		sprintf(file, "exp-0%d.png", explosions[i].animInc+1);
		drawSpriteFull(makeSimpleSprite(file), explosions[i].coord, scale, scale, 0.0);
	}

	// teleportations
	for(int i=0; i < MAX_TELE; i++) {
		if(!teleporters[i].valid) continue;

		for(int j=0; j < PARTICLE_DENSITY; j++) {
			char file[11];
			if(teleporters[i].isLemExp) {
				// pick exp color based on lemming
				Particle part = teleporters[i].particles[j];
				switch(part.lemColor) {
					case 0:
						sprintf(file, "lem-%s-exp-0%d.png", "red", part.color);
						break;
					case 1:
						sprintf(file, "lem-%s-exp-0%d.png", "blue", part.color);
						break;
					case 2:
						sprintf(file, "lem-%s-exp-0%d.png", "pink", part.color);
						break;
					case 3:
						sprintf(file, "lem-%s-exp-0%d.png", "orange", part.color);
						break;
				}

			}else{
				sprintf(file, "tele-%d.png", teleporters[i].particles[j].color);
			}
			drawSprite(makeSimpleSprite(file), teleporters[i].particles[j].coord);
		}
	}
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

			Particle p = { true, c, step, randomMq(0,2) };
			points[j] = p;
		}

		Tele t = { true, points, clock(), 250, false };
		teleporters[i] = t;
		break;
	}
}

void spawnLemExp(Coord c, int lemColor) {
	for(int i=0; i < MAX_TELE; i++) {
		if(teleporters[i].valid) continue;

		Particle *points = malloc(sizeof(Particle) * PARTICLE_DENSITY);

		// make points
		for(int j=0; j < PARTICLE_DENSITY; j++) {
			// calculate step here since much faster.
			double angle = randomMq(0, 360);
			Coord step = getAngleStep(angle, randomMq(1, 2), false);

			// when choosing color, only samll percentage should be hair.
			// int color = chance(90) ? randomMq(2,3) : 1;
			Particle p = { true, c, step, randomMq(1,3), lemColor };
			points[j] = p;
		}

		Tele t = { true, points, clock(), 650, true };
		teleporters[i] = t;
		break;
	}
}

void spawnExp(Coord c, bool smallExp) {
	// find a place to put it in our explosion array.
	for(int i=0; i < MAX_EXP; i++) {
		if(explosions[i].valid) continue;

		// make it
		Exp e = { true, c, 0, smallExp };
		explosions[i] = e;

		return;
	}
}

void spawnPuff(Coord c) {
	for(int p=0; p < MAX_PUFFS; p++){
		if(puffs[p].valid) continue;
		Puff puff = { true, c, clock(), 0 };
		puffs[p] = puff;
		break;
	}
}