#include <time.h>
#include "lem.h"
#include "player.h"
#include "assets.h"
#include "input.h"
#include "renderer.h"
#include "weapon.h"

const double MOVE_INC = 1;
const double BORDER = 10;

bool playerWalking = false;
static int playerIndex;

void walk() {

	Coord pos = lemmings[PLAYER_INDEX].coord;

	bool isRight = checkCommand(CMD_PLAYER_RIGHT);
	bool isLeft = checkCommand(CMD_PLAYER_LEFT);
	bool isDown = checkCommand(CMD_PLAYER_DOWN);
	bool isUp = checkCommand(CMD_PLAYER_UP);

	// Whether we're supposed to be "walking"
	if(isLeft || isRight || isUp || isDown)
		playerWalking = true;

	// Actually applying movement coordinates.
	if (isLeft && pos.x > BORDER) 
		pos.x -= MOVE_INC;
	if (isRight && pos.x < screenBounds.x-BORDER) 
		pos.x += MOVE_INC;
	if (isUp && pos.y > BORDER) 
		pos.y -= MOVE_INC;
	if (isDown && pos.y < screenBounds.y-BORDER)
		pos.y += MOVE_INC;

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

		lemmings[playerIndex].angle = degToRad(dir-90);
	}

	// apply position to the player lemming.
	lemmings[playerIndex].coord = pos;
}

void playerGameFrame(void) {
	playerWalking = false;

	// Shooting
	if(commands[CMD_SHOOT])
		shoot(0, radToDeg(lemmings[0].angle));

	walk();
}

void initPlayer() {
	playerIndex = spawnLem(makeCoord(20,20), 0, true);
}
