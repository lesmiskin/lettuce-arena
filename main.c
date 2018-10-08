#include <time.h>
#include <stdbool.h>
#include <assert.h>
#include "common.h"
#include "input.h"
#include "renderer.h"
#include "mysdl.h"
#include "player.h"
#include "assets.h"
#include "scene.h"
#include "lem.h"
#include "fx.h"
#include "enemy.h"
#include "weapon.h"
#include "hud.h"


// time limit + counter
// end screen when fraglimit is hit
// ability to have no player (spectator mode).
// shooting animation (overlaid muzzle-flash sprite)
// spawn points have a funny tile (e.g. quake 2 teleporter)
// player takes random spawn point FIRST.
// on spawn, lem should FACE inwards.

// sound effects
// add a wall block.

// game.cfg (fraglimit, time limit)
// botnames.cfg
// maps.cfg
// keys.cfg (arrows vs. wsad, ctrl vs other keys)

// simple punch to bash enemies around with

// simple title screen: headlamp rotation, joint explosion.
// demo mode comes up as attract mode!
// "-- DEMO MODE --" flasher






// powerup... invincibility.
// powerup... splash damage.
// gameplay: experiment with ammo + health bars

// machinegun
// dont overlap players
// title screen with headlamp rotation ala commander keen.
// ability to PUNCH (alex kidd style)
// stop enemy cheating with angular motion towards weapons
// annoying / unclear what happens when enemy grabs gun then shoots immediately and we respawn immediately
// ability to customise text (w/ colors)
// spinning rockets again.
// [easy] health packs
// [easy] spinning rocket sprites.
// [easy] 1-frame large muzzleflash.
// [easy] up/down walking sprites.
// player bumps back if part of an explosion.
// splash damage?





// ability to scroll, and enter different areas
// simple walls for strategy.
// enemies don't shoot if wall is in the way.


    // . . . .         . . . .
    // .                     .
    // .                     .
    // .                     .

    // .                     .
    // .                     .
    // .                     .
    // . . . .         . . . .

// QUAD DAMAGE powerup spawns infrequently in middle of map.

// chairs and computer desks lying around.
// office theme (worker sprites, chairs, desks with computers)
// office worker sprite.

// realistic walking styles:
// a) walk in a random direction for a random amount of time, then change direction (officespace-style).
// c) while walking, 50/50 chance to zig-zag on the way.
// OR
// a) pick a random place on the map, and walk to it.

// reactions:
// a) if pickup is available, high chance to walk to it.
// b) if weapon is available, higher chance to walk to it.
// c) if detect opponent within X threshold (random), then change direction AWAY from him.

static const char *GAME_TITLE = "Lettuce Arena v0.1";
const int ANIMATION_HZ = 1000 / 8;		//12fps
const int RENDER_HZ = 1000 / 60;		//60fps
const int GAME_HZ = 1000 / 60;			//60fps
const int FX_HZ = 1000 / 12;            //24fps

bool running = true;
SDL_Window *window = NULL;

Coord windowSize = { 320*3, 240*3 };   // 320x240

static void initSDL(void) {
    SDL_Init(SDL_INIT_AUDIO);
    if(!IMG_Init(IMG_INIT_PNG)) {
        fatalError("Fatal error", "SDL_Image did not initialise.");
    }
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fatalError("Fatal error", "SDL_Mixer did not initialise.");
    }
    SDL_InitSubSystem(SDL_INIT_VIDEO);
}

static void initWindow(void) {
    window = SDL_CreateWindow(
        GAME_TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        (int)windowSize.x,					//dimensions
        (int)windowSize.y,
        SDL_WINDOW_OPENGL
    );
    assert(window != NULL);
}

static void shutdownWindow(void) {
    if(window == NULL) return;			//OK to call if not yet setup (an established subsystem pattern elsewhere)

    SDL_DestroyWindow(window);
    window = NULL;
}

static void shutdownMain(void) {
    shutdownAssets();
    shutdownRenderer();
    shutdownWindow();

    SDL_Quit();
}

// spawn some monsters on the screen (green square)
// spawn a player on the screen (blue square)

int main()  {
    //Seed randomMq number generator
    srand(time(NULL));

    atexit(shutdownMain);

    initSDL();
    initWindow();
    initRenderer();
    initAssets();
    initEnemy();
    initScene();
    initWeapon();
    initHud();
    initPlayer();

	changeMode(MODE_GAME);

    long lastRenderFrameTime = clock();
    long lastGameFrameTime = lastRenderFrameTime;
    long lastAnimFrameTime = lastRenderFrameTime;
    long lastFxFrameTime = lastRenderFrameTime;

    //Main game loop (realtime)
    while(running){
        //Game frame
        if(timer(&lastGameFrameTime, GAME_HZ)) {
            pollInput();
            playerGameFrame();
            // enemyGameFrame();
            // enemyFxFrame();
			weaponGameFrame();
            lemGameFrame();
            fxGameFrame();
			sceneGameFrame();
			hudGameFrame();
            processSystemCommands();
        }

		//Animation frame
		if(timer(&lastAnimFrameTime, ANIMATION_HZ)) {
			// sceneAnimateFrame();
           lemAnimateFrame();
		}

        //Renderer frame
        double renderFPS;
        if(timer(&lastRenderFrameTime, RENDER_HZ)) {
			sceneRenderFrame();
            // enemyFxRenderFrame();
            // enemyRenderFrame();
			weaponRenderFrame();
            fxRenderFrame();
            lemRenderFrame();
            hudRenderFrame();
            // enemyDeathRenderFrame();

            updateCanvas();
        }
    }

    return 0;
}