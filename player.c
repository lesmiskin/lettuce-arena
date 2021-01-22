#include <time.h>
#include "lem.h"
#include "player.h"
#include "assets.h"
#include "input.h"
#include "renderer.h"
#include "weapon.h"
#include "state.h"

const double MOVE_INC = 1;

bool playerWalking = false;
int playerIndex;

const int BAR_SIZE = 10;

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
	// if (isLeft && pos.x > BORDER) 
	// 	pos.x -= MOVE_INC;
	// if (isRight && pos.x < screenBounds.x-BORDER) 
	// 	pos.x += MOVE_INC;
	// if (isUp && pos.y > BORDER + BAR_SIZE) 
	// 	pos.y -= MOVE_INC;
	// if (isDown && pos.y < screenBounds.y-BORDER)
	// 	pos.y += MOVE_INC;
	if (isLeft) 
		pos.x -= MOVE_INC;
	if (isRight) 
		pos.x += MOVE_INC;
	if (isUp) 
		pos.y -= MOVE_INC;
	if (isDown)
		pos.y += MOVE_INC;

	// Calculate what heading dir we're walking in (so we know what dir to
	// point our carry weapon in, among other reasons)
	if(playerWalking) {
		int dir;
		if (isLeft && isUp) 
			dir = 315; //nw
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

	// try to move.
	Move m = tryMove(pos, lemmings[PLAYER_INDEX].coord, PLAYER_INDEX);
	lemmings[playerIndex].coord = m.result;
}

void playerGameFrame(void) {
	if(practice || !usePlayer) return;

	playerWalking = false;

	// Shooting
	if(commands[CMD_SHOOT])
		shoot(0, radToDeg(lemmings[0].angle));

	walk();
}

void initPlayer() {
}

void startPlayer() {
	// playerIndex = spawnLem(spawns[randomMq(0, MAX_SPAWNS-1)], 0, true, 0, "player");
	// lemmings[0].ammo = 99;
	// lemmings[0].health = 999999;
	// lemmings[0].hasRock = true;
	if(CHEAT_GIVE_MACH) {
		lemmings[PLAYER_INDEX].weap = W_MACH;
		lemmings[PLAYER_INDEX].lastWeap = W_MACH;
		lemmings[PLAYER_INDEX].ammo = 999;
	}
	else if(CHEAT_GIVE_ROCK) {
		lemmings[PLAYER_INDEX].weap = W_ROCK;
		lemmings[PLAYER_INDEX].lastWeap = W_MACH;
		lemmings[PLAYER_INDEX].ammo = 999;
	}
}