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
const double ENEMY_SPEED = 0.5;
const double CHAR_BOUNDS = 15;
const double DIR_CHANGE = 250;

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

Coord calcDirOffset(Coord original, Dir dir) {
	Coord offset = zeroCoord();
	switch(dir) {
		case DIR_NORTH:
			offset = makeCoord(0, -ENEMY_SPEED);
			break;
		case DIR_NORTHEAST:
			offset = makeCoord(ENEMY_SPEED, -ENEMY_SPEED);
			break;
		case DIR_EAST:
			offset = makeCoord(ENEMY_SPEED, 0);
			break;
		case DIR_SOUTHEAST:
			offset = makeCoord(-ENEMY_SPEED, ENEMY_SPEED);
			break;
		case DIR_SOUTH:
			offset = makeCoord(0, ENEMY_SPEED);
			break;
		case DIR_SOUTHWEST:
			offset = makeCoord(-ENEMY_SPEED, ENEMY_SPEED);
			break;
		case DIR_WEST:
			offset = makeCoord(-ENEMY_SPEED, 0);
			break;
		case DIR_NORTHWEST:
			offset = makeCoord(-ENEMY_SPEED, -ENEMY_SPEED);
			break;
	}
	return mergeCoord(original, offset);
}

void enemyGameFrame(void) {
	for(int i=0; i < MAX_ENEMY; i++) {
		if(enemies[i].coord.x == 0) continue;

		// chase the player
		aiChaseFrame(i);
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
}

void spawnEnemy(EnemyType type, Coord coord) {
	if(enemyCount == MAX_ENEMY) return;

	Enemy e = {
		coord,
		randomMq(1, 4),
		type,
		clock(),
		DIR_NORTH,
		false
	};
	enemies[enemyCount++] = e;
}

void initEnemy(void) {
	//Make the enemies
	for(int i=0; i < INITIAL_ENEMIES; i++) {
		spawnEnemy(
			(EnemyType)randomMq(0, ENEMY_DRACULA),
			makeCoord(
				randomMq(0, screenBounds.x),
				randomMq(0, screenBounds.y)
			)
		);
	}
}
