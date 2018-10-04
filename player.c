#include <time.h>
#include "lem.h"
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
Coord pos = { 20, 20 };
int health = 100;
bool playerWalking = false;
long plastIdleTime;
static const int PANIM_HZ = 1000 / 4;
static int playerIndex;

void playerAnimateFrame(void) {
	// Stand still when not playerWalking.
	if(!playerWalking) {
		walkInc = STAND;
		return;
	}

	if(!timer(&plastIdleTime, PANIM_HZ)) return;

	walkInc = (walkInc == WALK_FRAMES) ? 1 : walkInc + 1;
}

void playerRenderFrame(void) {
	// // apply the animation frame
	// char frameFile[25];
	// sprintf(frameFile, "lem-pink-0%d.png", walkInc);

	// // are we traveling left or right?
	// SDL_RendererFlip flip = playerDir ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

	// // draw the sprite
	// Sprite sprite = makeFlippedSprite(frameFile, flip);
	// drawSprite(sprite, pos);

	// Sprite arrow = makeFlippedSprite(frameFile, flip);
	// // drawSprite(makeSimpleSprite("arrow.png"), deriveCoord(pos, -1, -13));
	// drawSprite(makeSimpleSprite("p1.png"), deriveCoord(pos, -1, -13));
	// // drawSprite(makeSimpleSprite("p1-arrow.png"), deriveCoord(pos, -1, -13));
}

const double BORDER = 10;

void playerGameFrame(void) {
	playerWalking = false;

	bool isRight = checkCommand(CMD_PLAYER_RIGHT);
	bool isLeft = checkCommand(CMD_PLAYER_LEFT);
	bool isDown = checkCommand(CMD_PLAYER_DOWN);
	bool isUp = checkCommand(CMD_PLAYER_UP);

	//NB: We still turn the player sprite where we can, even if we're hard
	// up against a screen bound.
	if (isLeft) {
		if(pos.x > BORDER) pos.x -= MOVE_INC;
		playerDir = false;
		playerWalking = true;
	}
	if (isRight) {
		if(pos.x < screenBounds.x-BORDER) pos.x += MOVE_INC;
		playerDir = true;
		playerWalking = true;
	}
	if (isUp && pos.y > BORDER) {
		pos.y -= MOVE_INC;
		playerWalking = true;
	}
	if (isDown && pos.y < screenBounds.y-BORDER) {
		pos.y += MOVE_INC;
		playerWalking = true;
	}

	// Calculate what heading dir we're walking in (so we know what dir to
	// point our carry weapon in, among other reasons)
	if(playerWalking) {
		int dir;
		if (isLeft && isUp) 
			dir = 305; //nw
		else if (isRight && isUp) 
			dir = 405; //ne
		else if (isLeft && isDown) 
			dir = 225; //sw
		else if (isRight && isDown) 
			dir = 135; //se
		else if (isLeft) 
			dir = 270; //w
		else if (isRight) 
			dir = 90; //e
		else if (isUp) 
			dir = 360; //n
		else if (isDown) 
			dir = 180; //s

		lemmings[playerIndex].en_idleTarget = degToRad(dir-90);
	}

	// apply position to the player lemming.
	lemmings[playerIndex].coord = pos;
}

void initPlayer() {
	health = 100;
	playerIndex = spawnLem(makeCoord(20,20), 0, true);
}