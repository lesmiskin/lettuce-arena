#include <stdlib.h>
#include <time.h>
#include "renderer.h"
#include "lem.h"
#include "input.h"
#include "weapon.h"
#include "state.h"
#include "enemy.h"

static Sprite letters[10];
static const int LETTER_WIDTH = 4;

void initHud(void) {
	//Pre-load font sprites.
	for(int i=0; i < 10; i++) {
		char textureName[50];
		sprintf(textureName, "font-%d.png", i);
		letters[i] = makeSimpleSprite(textureName);
	}
}

void writeText(int amount, Coord pos, bool centered) {
	if(amount == 0) {
		drawSpriteFull(letters[0], pos, 1, 1, 0, centered);
	}else{
		while(amount != 0) {
			drawSpriteFull(letters[amount % 10], pos, 1, 1, 0, centered);
			amount /= 10;
			pos.x -= LETTER_WIDTH;
		}
	}
}

void writeAmount(int amount, Coord pos) {
	char text[4];
	sprintf(text, "%d", amount);

	for(int i=0; i < strlen(text); i++) {
		char fontFile[50];

		sprintf(fontFile, "font-%c.png", text[i]);

		Sprite sprite = makeSimpleSprite(fontFile);
		drawSpriteFull(sprite, pos, 1, 1, 0, false);

		pos.x += 4;
	}
}

void writeFontFull(char *text, Coord pos, bool centerScreen, bool centerText) { 
	int stringLength = strlen(text);

	// center the text based on whole-screen, or existing coords
	if(centerScreen)
		pos.x = (screenBounds.x / 2) - ((stringLength * 4) / 2);
	if(centerText)
		pos.x -= ((stringLength * 4) / 2);

	for(int i=0; i < stringLength; i++) {
		//Print text if it's not a space.
		if(text[i] != ' ') {

			char fontFile[50];

			if(text[i] == '!') {
				sprintf(fontFile, "font-bang.png");
			}else{
				sprintf(fontFile, "font-%c.png", text[i]);
			}

			Sprite sprite = makeSimpleSprite(fontFile);
			drawSpriteFull(sprite, makeCoord(pos.x, pos.y), 1, 1, 0, false);

			pos.x += sprite.size.x - 1;
		}else{
			pos.x += 2;
		}
	}
}

void writeFont(char *text, Coord pos) {
	writeFontFull(text, pos, false, false);
}

void hudGameFrame(void) {
}

int compare_ints(const void *p, const void *q) {
    Lem x = *(const Lem *)p;
    Lem y = *(const Lem *)q;

    if (x.frags < y.frags)
        return 1;  // Return -1 if you want ascending, 1 if you want descending order. 
    else if (x.frags > y.frags)
        return -1;   // Return 1 if you want ascending, -1 if you want descending order. 

    return 0;
}

// the player stops BECAUSE they are no longer at INDEX 0.
// stop enemies from taking the SAME NAME.

void sort_ints(Lem *a, size_t n) {
    qsort(a, n, sizeof *a, &compare_ints);
}

const int firstLine = 2;
const int secondLine = 80;
const int scoreY = 100;
double pickupY = 0;

const int MSG_TIME = 2000;

void hudRenderFrame(void) {
	if(!inGame) return;

	Lem lem = lemmings[PLAYER_INDEX];

	writeFont("zone", makeCoord(10, 12));
	writeAmount(currentQuadrant, makeCoord(30, 12));

	bool showPosition = usePlayer && (checkCommand(CMD_SCORES) || gameover);

	// status bar background
	drawSpriteFull(makeSimpleSprite("bar.png"), zeroCoord(), 1, 1, 0, false);
	// drawSpriteFull(makeSimpleSprite("black.png"), zeroCoord(), screenBounds.x, 14, 0, false);

	// print who player killed
	if(usePlayer && lastPlayerKillIndex > -1) {
		if(isDue(clock(), lastPlayerKillTime, MSG_TIME)) {
			lastPlayerKillIndex = -1;
			lastPlayerKillTime = 0;
		}else{
			char killer[30];
			sprintf(killer, "you fragged %s", lemmings[lastPlayerKillIndex].name);
			writeFontFull(killer, makeCoord(135, firstLine), true, false);
		}
	}

	// print who killed us (unless we hit someone when dead! then show that instead)
	// additional logic is when showing at endgame, it goes away eventually.
	if(usePlayer && lem.dead && lastPlayerKillTime < lem.deadTime && !isDue(clock(), lem.deadTime, MSG_TIME)) {
		char killer[30];
		sprintf(killer, "fragged by %s",lemmings[lem.killer].name);
		writeFontFull(killer, makeCoord(135, firstLine), true, false);
		showPosition = true;
	}

	// Sort the scores
	Lem* scores = malloc(sizeof(Lem)*MAX_LEM);	
	memcpy(scores, lemmings, sizeof(lemmings));
	sort_ints(scores, 4);

	int boardPosition = -1;

	// find position in scoreboard.
	for(int i=0; i < MAX_LEM; i++) {
		if(scores[i].isPlayer) {
			boardPosition = i;
			break;
		}
	}

	// If we're tied for zero - mark us on the board anyway
	if(usePlayer && scores[0].frags == 0) boardPosition = 0;

	// show position statement
	if(showPosition) {
		char msg[30];
		char placing[10];

		// what phrasing should we use?
		switch(boardPosition) {
			case 0:
				sprintf(placing, "%s", "1st"); break;
			case 1:
				sprintf(placing, "%s", "2nd"); break;
			case 2:
				sprintf(placing, "%s", "3rd"); break;
			case 3:
				sprintf(placing, "%s", "4th");break;
		}

		sprintf(msg, "%s place with %d", placing, lem.frags);
		writeFontFull(msg, makeCoord(135, secondLine), true, false);
	}

	Lem p = lemmings[PLAYER_INDEX];

	// health
	writeAmount(p.active ? p.health : 0, makeCoord(20, firstLine));

	// ammo count
	if(p.weap > 0 && p.ammo > 0) {
		// writeFont("ammo", makeCoord(55, firstLine));
		if(p.weap == W_MACH) {
			drawSpriteFull(makeSimpleSprite("w_mach.png"), makeCoord(40,-3), 1, 1, 0, false);
		}else{
			drawSpriteFull(makeSimpleSprite("w_rock3-0.png"), makeCoord(40,-3), 1, 1, 0, false);
		}
		writeAmount(p.ammo, makeCoord(60, firstLine));
	}

	// frags
	writeText(fraglimit, makeCoord(262, firstLine), false);

	// highlight us if we are first, or second
	
	// in the lead.
	if(boardPosition == 0) {
		drawSpriteFull(makeSimpleSprite("white.png"), makeCoord(279,0), 16, 10, 0, false);
		drawSpriteFull(makeSimpleSprite("score.png"), makeCoord(280,1), 14, 8, 0, false);
	} else {
		drawSpriteFull(makeSimpleSprite("white.png"), makeCoord(299,0), 16, 10, 0, false);
		drawSpriteFull(makeSimpleSprite("score-2.png"), makeCoord(300,1), 14, 8, 0, false);
	}

	// first and second placements
	writeAmount(scores[0].frags, makeCoord(282, firstLine));
	writeAmount(boardPosition != 0 ? lem.frags : scores[1].frags, makeCoord(302, firstLine));

	// pickup plume
	if(!isDue(clock(), p.lastPickup, 300)) {
		// stop at the top for a little bit.
		// if(!isDue(clock(), p.lastPickup, 250)) {
			pickupY += 0.5;
		// }
		writeFontFull(p.lastItem, deriveCoord(p.pickupCoord, 0, -5 - (int)pickupY), false, true);
	}else{
		pickupY = 0;
	}

	// Scoreboard
	if(lem.dead || checkCommand(CMD_SCORES) || gameover) {
		int y =0;
		for(int i=0; i < MAX_LEM; i++) {
			// draw blue marker where we are.
			if(i == boardPosition) {
				drawSpriteFull(makeSimpleSprite("score.png"), makeCoord(130-9,scoreY + y-2), 75, 11, 0, false);
			}
			writeFont(scores[i].name, makeCoord(130, scoreY + y));
			writeAmount(scores[i].frags, makeCoord(180, scoreY + y));
			y += 10;
		}
	}

	// draw crosshair
	if(lem.weap > 0 && lem.active) {
		drawSpriteFull(
			makeSimpleSprite("cross-2.png"), 
			extendOnAngle(lem.coord, lem.angle, 35), 
			1, 1, 0, true
		);
	}

	// draw enemy indicators
	for(int i=0; i < MAX_LEM; i++) {
		int xPos = 3;
		int flip = SDL_FLIP_NONE;
		if(lemmings[i].quadrant > currentQuadrant){
			flip = SDL_FLIP_HORIZONTAL;
			xPos = screenBounds.x-3;
		} 
		else if(lemmings[i].quadrant < currentQuadrant){
			xPos = 3;
		} 
		else continue;

		drawSprite(
			makeFlippedSprite("indicator.png", flip), makeCoord(xPos, lemmings[i].coord.y)
		);
	}
}
