#include "scene.h"
#include "renderer.h"
#include "common.h"
#include "assets.h"
#include "weapon.h"
#include <time.h>

// MAP
int map[MAP_Y][MAP_X] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};


const int TILE_SIZE_X = 10;
const int TILE_SIZE_Y = 10;
static Sprite ground;
int currentQuadrant = 0;

typedef struct {
	Coord position;
	int layer;
	int brightness;
} Star;

static const double SCROLL_SPEED = 0.5;			//TODO: Should be in FPS.

// STARS
#define MAX_STARS 96
static bool starsBegun = false;
static int STAR_DELAY = 100;	//lower is greater.
static Star stars[MAX_STARS];
static long lastStarTime = 0;
static int starInc = 0;

Spawn spawns[MAX_SPAWNS];

Spawn makeSpawn(int quadrant, Coord coord) {
	Spawn s = { quadrant, coord };
	return s;
}

// ----------
// Weapons
// ----------
Weapon weapons[MAX_WEAPONS];

const double WEAP_RESPAWN_TIME = 5000;
long lastFlash;
bool flash;

const int ITEM_TIME_MIN = 1000;
const int ITEM_TIME_MAX = 3000;
long lastPowerupPickup;
long nextPowerupPickup;

int randomPowerupTime() {
	// NB: weird multiplication causes better spacing out of random instances.
	// i.e. we randomise on a lower set of sequences, then raise the time up again.
	return randomMq(ITEM_TIME_MIN / 1000, ITEM_TIME_MAX / 1000) * 1000;
}

void powerupPickup() {
	lastPowerupPickup = clock();
	nextPowerupPickup = randomPowerupTime();
}

bool isPowerupOnscreen() {
	for(int i=0; i < MAX_WEAPONS; i++) {
		if(!weapons[i].valid) continue;
		if(weapons[i].type == I_AMMO || weapons[i].type == I_HEALTH)
			return true;
	}
	return false;
}

void itemSpawn() {
	// find a slot
	for(int i=0; i < MAX_WEAPONS; i++) {
		if(weapons[i].valid) continue;

		// spawn it
		Weapon item = { randomMq(0,1), true, false, 0, 
			makeCoord(
				randomMq(TILE_SIZE_X, screenBounds.x-TILE_SIZE_X), 
				randomMq(TILE_SIZE_X, screenBounds.y-TILE_SIZE_X)), 
			chance(50) ? I_HEALTH : I_AMMO
		};
		weapons[i] = item;
		break;
	}
}

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

	// spawn powerups randomly
	if(!isPowerupOnscreen() && isDue(clock(), lastPowerupPickup, nextPowerupPickup)) {
		itemSpawn();
	}

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

	// Draw walls (TODO: Should do once and save as single image).
	for(int y=0; y < MAP_Y; y++) {
		for(int x=0; x < MAP_X; x++) {
			if(map[y][x] == 1) {
				drawSprite(makeSimpleSprite("keen-tile-2.png"), makeCoord(x*16, y*16));
			}
		}
	}

	// 1. Ability to draw walls for *each* of the quadrants (hidden and clippable if not in visble quadrant).
	// 2. Lem entities *cannot* move into square of a wall.
	// 3. Enemies *do not* target other lems when there is a wall in the firing line.


	// Draw weapons
	for(int i=0; i < MAX_WEAPONS; i++) {
		if(!weapons[i].valid || weapons[i].pickedUp) continue;
		if(weapons[i].quadrant != currentQuadrant) continue;

		if(timer(&lastFlash, 500)) flash = !flash;

		char file[23];
		if(weapons[i].type == W_MACH) 
			sprintf(file, "w_mach.png");
		else if(weapons[i].type == W_ROCK)
			sprintf(file, "w_rock-e.png");
		else if(weapons[i].type == I_HEALTH)
			sprintf(file, "powerup-health-5-%d.png", flash);
		else if(weapons[i].type == I_AMMO)
			sprintf(file, "powerup-ammo-3-%d.png", flash);

		// dancing weapons
		// Coord c = !flash ? weapons[i].coord : deriveCoord(weapons[i].coord, 0, 1);
		// drawSprite(makeSimpleSprite(file), c);

		Coord c = !flash ? weapons[i].coord : deriveCoord(weapons[i].coord, 0, 1);
		drawSprite(makeSimpleSprite(file), weapons[i].coord);
	}
}

//Should happen each time the scene is shown.
void initScene() {
	starsInit();
	makeGroundTexture();
}

void startScene() {
	// default weapon spawns.
	Weapon rock = { 0, true, false, 0,  makeCoord(screenBounds.x/2, 35), W_MACH };
	weapons[0] = rock;
	Weapon rock4 = { 0, true, false, 0, makeCoord(screenBounds.x/2, screenBounds.y-35), W_MACH };
	weapons[3] = rock4;

	Weapon rock2 = { 1, true, false, 0, makeCoord(35, screenBounds.y/2), W_ROCK };
	weapons[1] = rock2;
	Weapon rock3 = { 1, true, false, 0, makeCoord(screenBounds.x-35, screenBounds.y/2), W_ROCK };
	weapons[2] = rock3;

	// Weapon health = { true, false, 0, makeCoord(screenBounds.x-60, screenBounds.y/2), I_HEALTH };
	// weapons[4] = health;
	// Weapon amm = { true, false, 0, makeCoord(screenBounds.x-60, screenBounds.y-35), I_AMMO };
	// weapons[5] = amm;

	// seed the powerup counter
 	powerupPickup();
}