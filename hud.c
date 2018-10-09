#include <stdlib.h>
#include <time.h>
#include "renderer.h"
#include "lem.h"
#include "input.h"
#include "weapon.h"
#include "state.h"

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
	char text[3];
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
const int secondLine = 10;
const int scoreY = 100;

void hudRenderFrame(void) {
	Lem lem = lemmings[PLAYER_INDEX];

	bool showPosition = usePlayer && (checkCommand(CMD_SCORES) || gameover);

	// print who player killed
	if(usePlayer && lastPlayerKillIndex > -1) {
		if(isDue(clock(), lastPlayerKillTime, 1000)) {
			lastPlayerKillIndex = -1;
			lastPlayerKillTime = 0;
		}else{
			char killer[30];
			sprintf(killer, "you fragged %s", lemmings[lastPlayerKillIndex].name);
			writeFontFull(killer, makeCoord(135, firstLine), true, false);
			showPosition = true;
		}
	}

	// print who killed us (unless we hit someone when dead! then show that instead)
	// additional logic is when showing at endgame, it goes away eventually.
	if(usePlayer && lem.dead && lastPlayerKillTime < lem.deadTime && !isDue(clock(), lem.deadTime, 3000)) {
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

	// frags
	writeText(fraglimit, makeCoord(262, 3), false);

	// highlight us if we are first, or second
	
	// in the lead.
	if(boardPosition == 0) {
		drawSpriteFull(makeSimpleSprite("white.png"), makeCoord(279,0), 16, 13, 0, false);
		drawSpriteFull(makeSimpleSprite("score.png"), makeCoord(280,1), 14, 11, 0, false);
	} else {
		drawSpriteFull(makeSimpleSprite("white.png"), makeCoord(299,0), 16, 13, 0, false);
		drawSpriteFull(makeSimpleSprite("score-2.png"), makeCoord(300,1), 14, 11, 0, false);
	}

	// first and second placements
	writeAmount(scores[0].frags, makeCoord(282, 3));
	writeAmount(boardPosition != 0 ? lem.frags : scores[1].frags, makeCoord(302, 3));

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

	// ammo
	if(lem.hasRock && lem.active) {
		drawSprite(makeSimpleSprite("rocket-e.png"), makeCoord(10,6));
		writeAmount(lem.ammo, makeCoord(19, 3));

		// draw crosshair
		drawSpriteFull(
			makeSimpleSprite("cross.png"), 
			extendOnAngle(lem.coord, lem.angle, 35), 
			1, 1, 0, true
		);
	}
}