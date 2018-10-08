#include <stdlib.h>
#include "renderer.h"
#include "lem.h"
#include "input.h"
// #include "hud.h"
// #include "time.h"
// #include "scene.h"
// #include "enemy.h"

static Sprite letters[10];
static const int LETTER_WIDTH = 4;

void initHud(void) {
	//Pre-load font sprites.
	for(int i=0; i < 10; i++) {
		char textureName[50];
		sprintf(textureName, "font-%02d.png", i);
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

		sprintf(fontFile, "font-0%c.png", text[i]);

		Sprite sprite = makeSimpleSprite(fontFile);
		drawSpriteFull(sprite, pos, 1, 1, 0, false);

		pos.x += 4;
	}
}

void writeFont(char *text, Coord pos) {
	for(int i=0; i < strlen(text); i++) {
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

void hudGameFrame(void) {
}

int fraglimit = 30;

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

void hudRenderFrame(void) {
	Lem lem = lemmings[PLAYER_INDEX];

	// print who killed us
	if(lem.dead) {
		char killer[10];
		sprintf(killer, "fragged by %s",lemmings[lem.killer].name);
		writeFont(killer, makeCoord(135, 40));
	}

	// frags
	writeText(fraglimit, makeCoord(272, 3), false);
	drawSpriteFull(makeSimpleSprite("white.png"), makeCoord(289,0), 16, 13, 0, false);
	drawSpriteFull(makeSimpleSprite("score.png"), makeCoord(290,1), 14, 11, 0, false);
	writeAmount(lem.frags, makeCoord(292, 3));

	// Scoreboard
	if(lem.dead || checkCommand(CMD_SCORES)) {

		// Sort the scores
		Lem* scores = malloc(sizeof(Lem)*MAX_LEM);	
		memcpy(scores, lemmings, sizeof(lemmings));
		sort_ints(scores, 4);

		int y =0;
		for(int i=0; i < MAX_LEM; i++) {
			writeFont(scores[i].name, makeCoord(130, 80 + y));
			writeAmount(scores[i].frags, makeCoord(180, 80 + y));
			y += 10;
		}
	}

	// ammo
	if(lem.hasRock) {
		drawSprite(makeSimpleSprite("rocket-e.png"), makeCoord(10,6));
		writeAmount(lem.ammo, makeCoord(19, 3));

		// draw crosshair
		drawSpriteFull(
			makeSimpleSprite("cross.png"), 
			deriveCoord(lem.coord, 
				radToDeg(cos(lem.angle)) / 1.5, 
				radToDeg(sin(lem.angle)) / 1.5
			), 1, 1, 0, true);
	}

	// ability to PUNCH (alex kidd style)
	// PROPERLY center obit. text (take sum of all chars, divide width by two etc.)



	// writeFont("a", makeCoord(xoff, 		yoff));
	// writeFont("m", makeCoord(xoff + 5, 	yoff));
	// writeFont("m", makeCoord(xoff + 11,	yoff));
	// writeFont("o", makeCoord(xoff + 16, yoff));

	// writeFont("health", makeCoord(10, 10));
	// writeFont("health", makeCoord(10, 10));
}
