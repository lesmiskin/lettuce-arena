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

static const char *GAME_TITLE = "Lettuce Arena TEST";
const int ANIMATION_HZ = 1000 / 8;		//12fps
const int RENDER_HZ = 1000 / 60;		//60fps
const int GAME_HZ = 1000 / 60;			//60fps
const int FX_HZ = 1000 / 12;            //24fps

bool running = true;
SDL_Window *window = NULL;

Coord windowSize = { 320*2, 240*2 };   // 320x240

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
