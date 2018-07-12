#include "enemy.h"
#include "renderer.h"
#include "assets.h"
#include "time.h"
#include "player.h"
#include "ai.h"
#include "input.h"
// #include "hud.h"

#define WALK_FRAMES 4

long lastIdleTime;
int enemyCount = 0;
static const int ANIM_HZ = 1000 / 4;
const double CHAR_BOUNDS = 15;
const double DIR_CHANGE = 250;

const int INITIAL_ENEMIES = 3;
const double ENEMY_SPEED = 1;
Enemy enemies[MAX_ENEMY];

const double SHOT_SPEED = 2.0;
const double SHOT_RELOAD = 1000;
Shot shots[MAX_SHOTS];

// ----------
// Explosions
// ----------
#define MAX_EXP 20
const double BOUND = 20;

typedef struct {
	bool valid;
	Coord coord;
	int animInc;
} Exp;

Exp explosions[MAX_EXP];


// ----------
// Puffs
// ----------
typedef struct {
	bool valid;
	Coord coord;
	long spawntime;
} Puff;

#define MAX_PUFFS 1000
const double PUFF_FREQ = 75;
const double PUFF_DURATION = 350;

Puff puffs[MAX_PUFFS];


bool havingBreather(int enemyInc) {
	return enemies[enemyInc].lastBreather > 0;
}

// Random 8-way direction.
double randomEnemyAngle() {
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

bool onScreen(Coord coord, double threshold) {
	return inBounds(coord, makeRect(
			0 + threshold,
			0 + threshold,
			screenBounds.x - (threshold),
			screenBounds.y - (threshold)
	));
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
	return isDue(clock(), enemies[enemyIndex].lastShot, SHOT_RELOAD);
}

void fireAngleShot(int enemyIndex, double deg) {
	double rad = degToRad(deg);

	// find a spare projectile 
	for(int i=0; i < MAX_SHOTS; i++) {
		if(!shots[i].valid) {
			Coord origin = enemies[enemyIndex].coord;
			Coord shotStep = getAngleStep(rad, SHOT_SPEED, false);
			Shot s = { true, origin, shotStep, deg, 0, enemyIndex };
			shots[i] = s;
			break;
		}
	}
	enemies[enemyIndex].lastShot = clock();
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

void enemyGameFrame(void) {
	for(int i=0; i < MAX_ENEMY; i++) {
		if(enemies[i].coord.x == 0) continue;

		aiSmartFrame(i);

		// chase the player
		// aiChaseFrame(i);
	}

	// home shots towards target.
	for(int i=0; i < MAX_SHOTS; i++) {
		if(!shots[i].valid) continue;
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

			if(inBounds(shots[i].coord, makeSquareBounds(enemies[e].coord, BOUND))) {
				spawnExp(shots[i].coord);
				shots[i].valid = false;
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
				Puff puff = { true, shots[i].coord, clock() };
				puffs[p] = puff;
				break;
			}
		}
	}

	// make puffs go away after a short time.
	for(int p=0; p < MAX_PUFFS; p++){
		if(!puffs[p].valid) continue;
		if(isDue(clock(), puffs[p].spawntime, PUFF_DURATION)) {
			puffs[p].valid = false;
		}
	}
}

void enemyFxFrame() {
	// animate explosions at a custom FPS (hence why it's here)
	for(int i=0; i < MAX_EXP; i++) {
		if(!explosions[i].valid) continue;

		// finish explosion
		if(explosions[i].animInc == 3) {
			explosions[i].valid = false;
			continue;
		}

		explosions[i].animInc++;
	}
}

void enemyAnimateFrame(void) {
	if(!timer(&lastIdleTime, ANIM_HZ)) return;

	//Animate the enemies
	for(int i=0; i < MAX_ENEMY; i++) {
		if(enemies[i].coord.x == 0) continue;

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

void enemyRenderFrame(void){
	//Draw the enemies with the right animation frame.
	for(int i=0; i < MAX_ENEMY; i++) {
		if(enemies[i].coord.x == 0) continue;

		Sprite sprite;
		SDL_RendererFlip flip = SDL_FLIP_NONE;
		bool isUp = false;
		bool isDown = false;

		if(enemies[i].isRoaming) {
			//Flip in the direction we're roaming (default case takes care of left-facing)
			switch(enemies[i].roamDir) {
				case DIR_SOUTH:
					isDown = true;
					break;
				case DIR_SOUTHWEST:
				case DIR_WEST:
				case DIR_NORTHWEST:
					flip = SDL_FLIP_HORIZONTAL;
					break;
				case DIR_NORTH:
					isUp = true;
					break;
			}
		}else{
			isUp = enemies[i].coord.y > pos.y;
			isDown = enemies[i].coord.y < pos.y;
		}

		// char frameFile[25];

		// Choose graphic based on type.
		// switch(enemies[i].type) {
		// 	case ENEMY_DIGGER: {
		// 		strcpy(frameFile, "digger-walk-%02d.png");
		// 		flip = enemies[i].coord.x > pos.x ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
		// 		break;
		// 	}
		// 	case ENEMY_CTHULU: {
		// 		strcpy(frameFile, "cthulu-walk-%02d.png");
		// 		flip = enemies[i].coord.x > pos.x ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
		// 		break;
		// 	}
		// }

		// sprintf(frameFile, frameFile, enemies[i].animInc);

		// apply the animation frame
		char frameFile[25];

		// pick the right color
		switch(enemies[i].color) {
			case 0:
				sprintf(frameFile, "lem-orange-0%d.png", enemies[i].animInc);
				break;
			case 1:
				sprintf(frameFile, "lem-green-0%d.png", enemies[i].animInc);
				break;
			case 2:
				sprintf(frameFile, "lem-blue-0%d.png", enemies[i].animInc);
				break;
			case 3:
				sprintf(frameFile, "lem-blue-0%d.png", enemies[i].animInc);
				break;
		}

		// are we traveling left or right?
		double deg = radToDeg(enemies[i].idleTarget);
		flip = deg > 90 && deg < 270 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

		// draw the sprite
		sprite = makeFlippedSprite(frameFile, flip);
		drawSprite(sprite, enemies[i].coord);
	}

	// draw puffs
	for(int p=0; p < MAX_PUFFS; p++){
		if(!puffs[p].valid) continue;
		drawSprite(makeSimpleSprite("puff.png"), puffs[p].coord);
	}

	// draw shots 
	for(int i=0; i < MAX_SHOTS; i++) {
		if(!shots[i].valid) continue;
		Sprite sprite = makeSimpleSprite("rocket.png");
		drawSpriteFull(sprite, shots[i].coord, 1, shots[i].angle+90);
	}

	// draw explosions
	for(int i=0; i < MAX_EXP; i++) {
		if(!explosions[i].valid) continue;
		char file[10];
		sprintf(file, "exp-0%d.png", explosions[i].animInc+1);
		drawSprite(makeSimpleSprite(file), explosions[i].coord);
	}
}

void spawnEnemy(EnemyType type, Coord coord) {
	if(enemyCount == MAX_ENEMY) return;

	Enemy e = {
		coord,
		randomMq(1, 4),
		type,
		clock(),
		DIR_NORTH,
		false,
		0,
		randomEnemyAngle(),
		clock(),
		500,
		0,
		enemyCount
	};
	enemies[enemyCount++] = e;
}

void initEnemy(void) {
	//Make the enemies
	for(int i=0; i < INITIAL_ENEMIES; i++) {
		spawnEnemy(
			(EnemyType)randomMq(0, ENEMY_DRACULA),
			makeCoord(
				randomMq(20, screenBounds.x-20),
				randomMq(20, screenBounds.y-20)
			)
		);
	}
}
