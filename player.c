#include <time.h>
#include "player.h"
#include "assets.h"
#include "input.h"
#include "enemy.h"
#include "renderer.h"
// #include "hud.h"
// #include "scene.h"

double MOVE_INC = 1;

#define WALK_FRAMES 4

#define STAND 2

bool playerDir = true;		// facing right
int walkInc = STAND;		// standing frame.
Coord pos = { 25, 50 };
double health = 100;
bool walking = false;
long plastIdleTime;
static const int PANIM_HZ = 1000 / 4;

void playerAnimateFrame(void) {
	// Stand still when not walking.
	if(!walking) {
		walkInc = STAND;
		return;
	}

	if(!timer(&plastIdleTime, PANIM_HZ)) return;

	walkInc = (walkInc == WALK_FRAMES) ? 1 : walkInc + 1;
}

void playerRenderFrame(void) {
	// apply the animation frame
	char frameFile[25];
	sprintf(frameFile, "lem-0%d.png", walkInc);

	// are we traveling left or right?
	SDL_RendererFlip flip = playerDir ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

	// draw the sprite
	Sprite sprite = makeFlippedSprite(frameFile, flip);
	drawSprite(sprite, pos);
}

const double BORDER = 10;

void playerGameFrame(void) {
	walking = false;

	//NB: We still turn the player sprite where we can, even if we're hard
	// up against a screen bound.
	if (checkCommand(CMD_PLAYER_LEFT)) {
		if(pos.x > BORDER) pos.x -= MOVE_INC;
		playerDir = false;
		walking = true;
	}
	if (checkCommand(CMD_PLAYER_RIGHT)) {
		if(pos.x < screenBounds.x-BORDER) pos.x += MOVE_INC;
		playerDir = true;
		walking = true;
	}
	if (checkCommand(CMD_PLAYER_UP) && pos.y > BORDER) {
		pos.y -= MOVE_INC;
		walking = true;
	}
	if (checkCommand(CMD_PLAYER_DOWN) && pos.y < screenBounds.y-BORDER) {
		pos.y += MOVE_INC;
		walking = true;
	}
}

void initPlayer() {
	pos.x = 25;
	pos.y = 50;
	health = 100;
}
