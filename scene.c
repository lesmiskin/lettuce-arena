#include "scene.h"
#include "renderer.h"
#include "common.h"
#include "assets.h"
#include <time.h>

const int TILE_SIZE_X = 10;
const int TILE_SIZE_Y = 10;
static Sprite ground;

typedef struct {
	Coord position;
	int layer;
	int brightness;
} Star;

static const double SCROLL_SPEED = 0.5;			//TODO: Should be in FPS.

//STARS
#define MAX_STARS 96
static bool starsBegun = false;
static int STAR_DELAY = 100;	//lower is greater.
static Star stars[MAX_STARS];
static long lastStarTime = 0;
static int starInc = 0;


// ----------
// Weapons
// ----------
const int WEAP_ROCKET = 0;
Weapon weapons[MAX_WEAPONS];

const double WEAP_RESPAWN_TIME = 5000;
long lastFlash;
bool flash;


static void makeGroundTexture() {
	//Init the SDL texture
	SDL_Texture *groundTexture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGB24,
		SDL_TEXTUREACCESS_TARGET,
		(int)1024,
		(int)768
	);
	SDL_SetTextureBlendMode(groundTexture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureAlphaMod(groundTexture, 255);

	// SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255);
	// SDL_RenderClear(renderer);

	//Prepare sprite, and change the rendering target to our above texture.
	// Sprite tile = makeSimpleSprite("space.png");
	Sprite tile = makeSimpleSprite("base-tile.png");
	SDL_SetRenderTarget(renderer, groundTexture);

	// drawSprite(tile, makeCoord(320/2, 240/2));

	//Draw the tiles out onto the texture.
	for(int x=0; x < 320; x += TILE_SIZE_X) {
		if(x < 10 || x >= 320-10) continue;
		for (int y = 0; y < 240; y += TILE_SIZE_Y) {
			if(y < 10 || y >= 240-10) continue;

			// hole in middle
			if(x == 160 && y == 120) continue;
			if(x == 150 && y == 120) continue;
			if(x == 160 && y == 110) continue;
			if(x == 150 && y == 110) continue;

			// top hole
			if(x == 150 && y == 10) continue;
			if(x == 160 && y == 10) continue;

			// bottom hole
			if(x == 150 && y == 220) continue;
			if(x == 160 && y == 220) continue;

			// left hole
			if(x == 10 && y == 110) continue;
			if(x == 10 && y == 120) continue;

			// right hole
			if(x == 300 && y == 110) continue;
			if(x == 300 && y == 120) continue;

			drawSpriteFull(tile, makeCoord(x, y), 1, 1, 0, false);
		}
	}

	//Darken
	SDL_SetTextureColorMod(groundTexture, 128, 128, 128);

	//Switch rendering back to the normal renderBuffer target.
	SDL_SetRenderTarget(renderer, renderBuffer);

	ground = makeSprite(groundTexture, zeroCoord(), SDL_FLIP_NONE);
}

void sceneAnimateFrame() {
}

void sceneGameFrame() {
	// respawn weapons
	for(int i=0; i < MAX_WEAPONS; i++) {
		if(!weapons[i].valid || !weapons[i].pickedUp) continue;
		if(isDue(clock(), weapons[i].lastPickup, WEAP_RESPAWN_TIME)) {
			weapons[i].pickedUp = false;
		}
	}

	//Scroll stars.
	for(int i=0; i < MAX_STARS; i++) {
		//Different star 'distances' scroll at different speeds.
		stars[i].position.x -=
			stars[i].brightness == 0 ? 0.75 : stars[i].brightness == 1 ? 1 : 1.25;
	}
}

void starsInit() {
	for(int i=0; i < 40; i++) {
		Star star = {
			makeCoord(
				randomMq(0, screenBounds.x),
				randomMq(0, screenBounds.y)
			),
			randomMq(0, 2),		//layer
			randomMq(0, 2),		//brightness
		};
		stars[i++] = star;
	}
}

void renderStars() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	//Spawn stars based on designated density.
	if(timer(&lastStarTime, STAR_DELAY)) {
		Star star = {
			makeCoord(
				screenBounds.x,
				randomMq(0, screenBounds.y)  //spawn across the width of the screen
			),
			randomMq(0, 2),		//layer
			randomMq(0, 2),		//brightness
		};

		starInc = starInc == MAX_STARS ? 0 : starInc++;
		stars[starInc++] = star;
	}

	//Render stars.
	for(int i=0; i < MAX_STARS; i++) {
		Sprite sprite = makeSprite(getTexture(
			stars[i].layer == 0 ? "star-dark.png" : stars[i].layer == 1 ? "star-dim.png" : "star-bright.png"
		), zeroCoord(), SDL_FLIP_NONE);

		drawSprite(sprite, stars[i].position);
	}
}

void sceneRenderFrame() {
	renderStars();
	drawSprite(ground, makeCoord(1024/2, 768/2));

	// Draw weapons
	for(int i=0; i < MAX_WEAPONS; i++) {
		if(!weapons[i].valid || weapons[i].pickedUp) continue;

		if(timer(&lastFlash, 500)) flash = !flash;

		char file[12];
		sprintf(file, "w_rock3-%d.png", flash);

		// dancing weapons
		// Coord c = !flash ? weapons[i].coord : deriveCoord(weapons[i].coord, 0, 1);
		// drawSprite(makeSimpleSprite(file), c);

		Coord c = !flash ? weapons[i].coord : deriveCoord(weapons[i].coord, 0, 1);
		drawSprite(makeSimpleSprite(file), weapons[i].coord);
	}

	// ROCKS
	const int BRICK = 10;

	// drawSpriteFull(makeSimpleSprite("rock7.png"), makeCoord(BRICK*13, BRICK*8), 1, 1, 0, false);
	// drawSpriteFull(makeSimpleSprite("rock7.png"), makeCoord(BRICK*13, BRICK*9), 1, 1, 0, false);
	// drawSpriteFull(makeSimpleSprite("rock7.png"), makeCoord(BRICK*13, BRICK*10), 1, 1, 0, false);
	// drawSpriteFull(makeSimpleSprite("rock7.png"), makeCoord(BRICK*13, BRICK*11), 1, 1, 0, false);
    // 32 x 24
	// for(int i=0; i < screenBounds.x; i += 10) {
	// 	drawSpriteFull(makeSimpleSprite("rock7.png"), makeCoord(i, 1), 1, 1, 0, false);
	// }
	// for(int i=0; i < screenBounds.x; i += 10) {
	// 	drawSpriteFull(makeSimpleSprite("rock7.png"), makeCoord(i, 240-10), 1, 1, 0, false);
	// }
}

//Should happen each time the scene is shown.
void initScene() {
	Weapon rock = { true, false, 0,  makeCoord(160-30, 120-30), WEAP_ROCKET };
	weapons[0] = rock;

	Weapon rock2 = { true, false, 0, makeCoord(160+30, 120-30), WEAP_ROCKET };
	weapons[1] = rock2;

	Weapon rock3 = { true, false, 0, makeCoord(160-30, 120+30), WEAP_ROCKET };
	weapons[2] = rock3;

	Weapon rock4 = { true, false, 0, makeCoord(160+30, 120+30), WEAP_ROCKET };
	weapons[3] = rock4;

	starsInit();
	makeGroundTexture();
}
