#include <stdlib.h>
#include "renderer.h"
#include "lem.h"
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
			drawSprite(sprite, makeCoord(pos.x, pos.y));

			if(text[i] == 'q') {
				pos.x += 4;
			}else if(text[i] == 'w' || text[i] == 'm') {
				pos.x += 5;
			}else if(text[i] == 'o') {
				pos.x += 5;
			}else if(text[i] == 'u') {
				pos.x += 5;
			}else if(text[i] == 'i' || text[i] == 'e') {
				pos.x += 3;
			}else{
				pos.x += sprite.size.x - 1;
			}
		}else{
			pos.x += 2;
		}
	}
}

void hudGameFrame(void) {
}

int fraglimit = 30;

void hudRenderFrame(void) {
	Lem lem = lemmings[PLAYER_INDEX];

	// print who killed us
	if(lem.dead) {
		char killer[10];
		sprintf(killer, "fragged by %s",lemmings[lem.killer].name);
		writeFont(killer, makeCoord(135, 5));
	}

	// frags
	drawSpriteFull(makeSimpleSprite("white.png"), makeCoord(289,0), 16, 13, 0, false);
	drawSpriteFull(makeSimpleSprite("score.png"), makeCoord(290,1), 14, 11, 0, false);
	// drawSprite(makeSimpleSprite("cross.png"), makeCoord(5,lem.frags));
	writeAmount(lem.frags, makeCoord(292, 3));

	writeText(fraglimit, makeCoord(272, 3), false);

	// ammo
	if(lem.hasRock) {
		drawSprite(makeSimpleSprite("rocket-e.png"), makeCoord(30,5));
		writeText(lem.ammo, makeCoord(39, 5), false);
	}


	// writeFont("a", makeCoord(xoff, 		yoff));
	// writeFont("m", makeCoord(xoff + 5, 	yoff));
	// writeFont("m", makeCoord(xoff + 11,	yoff));
	// writeFont("o", makeCoord(xoff + 16, yoff));

	// writeFont("health", makeCoord(10, 10));
	// writeFont("health", makeCoord(10, 10));
}
