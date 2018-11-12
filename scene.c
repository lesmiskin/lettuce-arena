#include "scene.h"
#include "renderer.h"
#include "common.h"
#include "assets.h"
#include <time.h>

const int TILE_SIZE_X = 48;
const int TILE_SIZE_Y = 48;
static Sprite ground;

typedef struct {
	Coord position;
	int layer;
	int brightness;
} Star;

static const double SCROLL_SPEED = 0.5;			//TODO: Should be in FPS.

//STARS
#define MAX_STARS 64
static bool starsBegun = false;
static int STAR_DELAY = 150;	//lower is greater.
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

	//Prepare sprite, and change the rendering target to our above texture.
	// Sprite tile = makeSimpleSprite("space.png");
	Sprite tile = makeSimpleSprite("space.png");
	SDL_SetRenderTarget(renderer, groundTexture);

	// drawSprite(tile, makeCoord(320/2, 240/2));

	//Draw the tiles out onto the texture.
	for(int x=0; x < 320; x += TILE_SIZE_X) {
		for (int y = 0; y < 240; y += TILE_SIZE_Y) {
			drawSprite(tile, makeCoord(x, y));
		}
	}

	//Darken
	SDL_SetTextureColorMod(groundTexture, 64, 64, 96);

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
		stars[i].position.y +=
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
				randomMq(0, screenBounds.x),  //spawn across the width of the screen
				0
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
//	drawSprite(ground, makeCoord(1024/2, 768/2));

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

	const int BRICK = 10;

	// drawSpriteFull(makeSimpleSprite("rock7.png"), makeCoord(BRICK*13, BRICK*8), 1, 1, 0, false);
	// drawSpriteFull(makeSimpleSprite("rock7.png"), makeCoord(BRICK*13, BRICK*9), 1, 1, 0, false);
	// drawSpriteFull(makeSimpleSprite("rock7.png"), makeCoord(BRICK*13, BRICK*10), 1, 1, 0, false);
	// drawSpriteFull(makeSimpleSprite("rock7.png"), makeCoord(BRICK*13, BRICK*11), 1, 1, 0, false);

    // 32 x 24

	for(int i=0; i < screenBounds.x; i += 10) {
		drawSpriteFull(makeSimpleSprite("rock7.png"), makeCoord(i, 1), 1, 1, 0, false);
	}

	for(int i=0; i < screenBounds.x; i += 10) {
		drawSpriteFull(makeSimpleSprite("rock7.png"), makeCoord(i, 240-10), 1, 1, 0, false);
	}
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
//	makeGroundTexture();
}
