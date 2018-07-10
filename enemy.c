
#include "enemy.h"
#include "renderer.h"
#include "assets.h"
#include "time.h"
#include "player.h"
#include "ai.h"
// #include "hud.h"

//TODO: Enemy movement to be bound to a movementDir? (e.g. 8-way, not x-way)
//TODO: Enemy standing frames when not moving.
//TODO: Enemies face in direction they're walking in, when roaming.
//TODO: Up and down enemy walking graphics.

#define WALK_FRAMES 4
 Enemy enemies[MAX_ENEMY];
long lastIdleTime;
int enemyCount = 0;
const int INITIAL_ENEMIES = 50;
const int IDLE_HZ = 1000 / 4;
const double ENEMY_SPEED = 1;
const double CHAR_BOUNDS = 15;
const double DIR_CHANGE = 250;
Shot shots[MAX_SHOTS];

const double SHOT_SPEED = 6.0;

// Random 8-way direction.
double randomEnemyAngle() {
	// double angle = 270;
	// return degToRad(angle);

	double deg;
	switch(randomMq(0, 7)){
		case 1:
			deg = 45;
			break;
		// down
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
		// right
		default:
			deg = 0;
			break;
	}

	// deg = 45;

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

void fireShot(int enemyIndex, Coord target) {

	// can we fire? (e.g. are we still waiting for recoil on last shot)
	if(isDue(clock(), enemies[enemyIndex].lastShot, 1000)) {
		// find a spare projectile 
		for(int i=0; i < MAX_SHOTS; i++) {
			if(!shots[i].valid) {
				Coord origin = enemies[enemyIndex].coord;
				Coord shotStep = getStep(origin, target, SHOT_SPEED, false);
				Shot s = { true, origin, shotStep, 0, false};
				shots[i] = s;
				break;
			}
		}
		enemies[enemyIndex].lastShot = clock();
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

		// TODO: turn off shot if hit player.

		// turn off shots out of range.
		if(!onScreen(shots[i].coord, 0)) 
			shots[i].valid = false;
	}
}

void enemyAnimateFrame(void) {
	if(!timer(&lastIdleTime, IDLE_HZ)) return;

	//Animate the enemies
	for(int i=0; i < MAX_ENEMY; i++) {
		if(enemies[i].coord.x == 0) continue;

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
		// 	case ENEMY_WOLFMAN: {
		// 		if(isUp) {
		// 			strcpy(frameFile, "werewolf-walk-up-%02d.png");
		// 		}else if(isDown) {
		// 			strcpy(frameFile, "werewolf-walk-down-%02d.png");
		// 		}else{
		// 			strcpy(frameFile, "werewolf-walk-%02d.png");
		// 			flip = enemies[i].coord.x > pos.x ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
		// 		}
		// 		break;
		// 	}
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
		// 	case ENEMY_DRACULA: {
		// 		if(isUp) {
		// 			strcpy(frameFile, "dracula-walk-up-%02d.png");
		// 			flip = SDL_FLIP_HORIZONTAL;	//hack
		// 		}else if(isDown){
		// 			strcpy(frameFile, "dracula-walk-down-%02d.png");
		// 			flip = SDL_FLIP_HORIZONTAL;	//hack
		// 		}else{
		// 			strcpy(frameFile, "dracula-walk-%02d.png");
		// 			flip = enemies[i].coord.x > pos.x ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
		// 		}
		// 		break;
		// 	}
		// }

		// sprintf(frameFile, frameFile, enemies[i].animInc);

		char frameFile[25] = "enemy.png";

		sprite = makeFlippedSprite(frameFile, flip);
		drawSprite(sprite, enemies[i].coord);
	}

	// draw shots 
	for(int i=0; i < MAX_SHOTS; i++) {
		if(!shots[i].valid) continue;
		drawSprite(makeSimpleSprite("dirt.png"), shots[i].coord);
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
		500
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
