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
#include "state.h"

// NW rockets not working for enemies :S

// figure out why enemy shots end up going in WOBBLY dir

// enemy lems get low health, and bar turns black :p

// WHITE PUFFS
// VIVID YELLOW BULLETS.

// machinegun holding sprite.
// muzzle flashes

// auto-strafe when button held down?!

// spawn point textures
// moving / swaying weapons when idle

// puff of colored smoke when machinegun hits
// HURT flicker animation when hit by machinegun
// muzzle flash for both weapons

// STUN lemming when killed by machinegun (not explode)
// up/down lemming sprites

// CRATES with powerups (invince, quad damage, health)
// crates can be shot








// Scrolling stars are VERY unoptimised. replace with scrolling bg's?
// Fade in / fade out




// up/down movement gif's

// MAP ------------------------------
// starry sky "out the window" (scrolling ala hexxagon)
// q3dm7 style, metal platforms with edges.

// WALLS -----------------------------
// wall segments in middle of the map.
// array-style wall definition.
// monsters wont shoot through walls.
// monsters will deflect if bumping against a wall.

// zelda-esque map sections, inspired from q3dm17 (outside area, inner area etc.)
// BUG: enemies can (still) get stuck on one another.







// enemies walk through players when homing into rockets.
// enemies cheat by homing directly to rockets rather than legally walking.


// rocket is too high-color, reduce it to be more pixelly.
// quad damage powerup
// pools of lava















// - if we are NEAR a wall (i.e. block of two or more, then MOVE AROUND IT)
// directional gibs based on rocket direction.
// death/explosion gets pushed back, like in regular hits.

// powerup dropins (ala worms)
// all pickups are in wood crates that you bash open (ala gta)

// inspiration: worms (names, crates, powerups)
// inspiration: quake 3 arena (respawning, pace, weapons, scoreboards)
// inspiration: gta (crates, machinegun)
// inspiration: lemmings (explosions, sprites)




// when killed, or KILL, show that lemming's name in yellow.
// demo mode (game can play itself -- spectator mode)
// shooting animation (overlaid muzzle-flash sprite)
// spawn points have a funny tile (e.g. quake 2 teleporter)

// icon when killed, or we frag someone
// "frags" and "deaths"
// no. of deaths on scoreboard
// blue lettering when killing / 1st place.
// red lettering when dead

// frag plumes above enemies that just killed someone

// title screen:
// - double-sized lettering
// - flashing "press any key"
// - rippling bright lettering.









// ((
// gameplay tweaks: player speed.
// gameplay tweaks: bounding box.
// ))

// sound effects
// add a wall block.


// time limit + counter

// game.cfg (fraglimit, time limit)
// botnames.cfg
// maps.cfg
// keys.cfg (arrows vs. wsad, ctrl vs other keys)

// simple punch to bash enemies around with






// powerup... invincibility.
// powerup... splash damage.
// gameplay: experiment with ammo + health bars



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
	SDL_ShowCursor(SDL_DISABLE);
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

	startGame();

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
			weaponGameFrame();
            lemGameFrame();
            fxGameFrame();
			sceneGameFrame();
			hudGameFrame();
			stateFrame();
            processSystemCommands();
        }

		//Animation frame
		if(timer(&lastAnimFrameTime, ANIMATION_HZ)) {
           lemAnimateFrame();
		}

        //Renderer frame
        double renderFPS;
        if(timer(&lastRenderFrameTime, RENDER_HZ)) {
			sceneRenderFrame();
			weaponRenderFrame();
            fxRenderFrame();
            lemRenderFrame();
            hudRenderFrame();

            updateCanvas();
        }
    }

    return 0;
}