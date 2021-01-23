#include <stdbool.h>
#include "common.h"
#include "input.h"
#include "lem.h"
#include "weapon.h"
#include "hud.h"
#include "enemy.h"
#include "player.h"
#include "state.h"

bool commands[MAX_COMMANDS];

bool checkCommand(int commandFlag) {
    return commands[commandFlag];
}

void pollInput(void) {
    SDL_PumpEvents();
    const Uint8 *keysHeld = SDL_GetKeyboardState(NULL);

	// FIX: Whole notion of storing commands and cancelling them out is wrong :p
	// they are flag-number based, NOT index-based.

    //We're on a new frame, so clear all previous checkCommand (not key) states (i.e. set to false)
	for(int i=0; i < MAX_COMMANDS; i++) commands[i] = false;

	// HACK
	commands[CMD_SCORES] = false;

    //Respond to SDL events, or key presses (not holds)
    SDL_Event event;
    while(SDL_PollEvent(&event) != 0) {
		switch (event.type) {
			case SDL_QUIT:
				commands[CMD_QUIT] = true;
				break;
			//Presses
			case SDL_KEYDOWN: {
				//Ignore held keys.
				if (event.key.repeat) break;

				SDL_Keycode keypress = event.key.keysym.scancode;

				// Restart the game.
				if (gameover && (
					keypress == SDL_SCANCODE_LCTRL || 
					keypress == SDL_SCANCODE_RETURN || 
					keypress == SDL_SCANCODE_SPACE
				)) {
					commands[CMD_RESTART] = true;
				}

				//Exit to title.
				if(keypress == SDL_SCANCODE_ESCAPE) {
					if(inGame) {
						changeMode(MODE_TITLE);
					}else{
						commands[CMD_QUIT] = true;
					}
					return;
				}

				// Start the game from title screen.
				if(!inGame && introReady) {
					changeMode(MODE_GAME);
				}
			}
		}
	}

	//Combat keys
	if(inGame) {
		if(keysHeld[SDL_SCANCODE_LEFT])
			commands[CMD_PLAYER_LEFT] = true;
		else if(keysHeld[SDL_SCANCODE_RIGHT])
			commands[CMD_PLAYER_RIGHT] = true;

		if(keysHeld[SDL_SCANCODE_UP])
			commands[CMD_PLAYER_UP] = true;
		else if(keysHeld[SDL_SCANCODE_DOWN])
			commands[CMD_PLAYER_DOWN] = true;

		if(keysHeld[SDL_SCANCODE_LCTRL] || keysHeld[SDL_SCANCODE_RCTRL])
			commands[CMD_SHOOT] = true;

		if(keysHeld[SDL_SCANCODE_TAB])
			commands[CMD_SCORES] = true;
	}
}

void processSystemCommands(void) {
    if(checkCommand(CMD_QUIT)) quit();
}