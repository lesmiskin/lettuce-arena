#include "scene.h"
#include "renderer.h"
#include "common.h"
#include "assets.h"
#include <time.h>

const int TILE_SIZE_X = 2;
const int TILE_SIZE_Y = 2;
static Sprite ground;


// ----------
// Weapons
// ----------
const int WEAP_ROCKET = 0;
Weapon weapons[MAX_WEAPONS];

const double WEAP_RESPAWN_TIME = 1000;
long lastFlash;
bool flash;


static void makeGroundTexture() {
	//Init the SDL texture
	SDL_Texture *groundTexture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGB24,
		SDL_TEXTUREACCESS_TARGET,
		(int)800,
		(int)600
	);

	//Prepare sprite, and change the rendering target to our above texture.
	Sprite tile = makeSimpleSprite("floor.png");
	SDL_SetRenderTarget(renderer, groundTexture);

	//Draw the tiles out onto the texture.
	for(int x=0; x < 320; x += TILE_SIZE_X) {
		for (int y = 0; y < 240; y += TILE_SIZE_Y) {
			drawSprite(tile, makeCoord(x, y));
		}
	}

	//Darken
//	SDL_SetTextureColorMod(groundTexture, 64, 64, 128);

	//Switch rendering back to the normal renderBuffer target.
	SDL_SetRenderTarget(renderer, renderBuffer);

	ground = makeSprite(groundTexture, zeroCoord(), SDL_FLIP_NONE);
}

void sceneAnimateFrame() {
}

void sceneGameFrame() {
}

void sceneRenderFrame() {
	// Draw weapons
	for(int i=0; i < MAX_WEAPONS; i++) {
		if(!weapons[i].valid || weapons[i].pickedUp) continue;
		if(timer(&lastFlash, 500)) flash = !flash;

		char file[12];
		sprintf(file, "w_rock-%d.png", flash);
		drawSprite(makeSimpleSprite(file), weapons[i].coord);
	}
}

//Should happen each time the scene is shown.
void initScene() {
	Weapon rock = { true, false, 0, makeCoord(100, 100), WEAP_ROCKET };
	weapons[0] = rock;

	Weapon rock2 = { true, false, 0, makeCoord(200, 100), WEAP_ROCKET };
	weapons[1] = rock2;

	Weapon rock3 = { true, false, 0, makeCoord(100, 200), WEAP_ROCKET };
	weapons[2] = rock3;

	Weapon rock4 = { true, false, 0, makeCoord(200, 200), WEAP_ROCKET };
	weapons[3] = rock4;

//	makeGroundTexture();
}
