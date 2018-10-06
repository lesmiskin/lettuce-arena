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
} Exp;

#define MAX_EXP 20
const double BOUND = 20;
Exp explosions[MAX_EXP];
long lastExpFrame;


typedef struct {
	bool valid;
	Coord coord;
	Coord step;
	int color;
} Particle;

typedef struct {
    bool valid;
    Particle* particles;
    long spawnTime;
} Tele;

#define PARTICLE_DENSITY 20
#define MAX_TELE 4
const double PARTICLE_SPEED = 1.2;
const int PARTICLE_TIME = 250;


Tele teleporters[MAX_TELE];

void fxGameFrame() {
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

		if(isDue(clock(), teleporters[i].spawnTime, PARTICLE_TIME)) {
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
			char file[11];
			sprintf(file, "tele-%d.png", teleporters[i].particles[j].color);
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