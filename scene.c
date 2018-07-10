#include "scene.h"
#include "renderer.h"
#include "common.h"
#include "assets.h"
// #include "hud.h"

const int TILE_SIZE_X = 2;
const int TILE_SIZE_Y = 2;
static Sprite ground;

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
//	drawSprite(ground, makeCoord(320, 240));
}

//Should happen each time the scene is shown.
void initScene() {
//	makeGroundTexture();
}
