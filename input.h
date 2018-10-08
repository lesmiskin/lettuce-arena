#ifndef INPUT_H
#define INPUT_H

#include "common.h"

#define MAX_COMMANDS 20

extern void pollInput(void);
extern void processSystemCommands(void);
extern bool checkCommand(int commandFlag);
extern bool commands[MAX_COMMANDS];

typedef enum {
    CMD_QUIT = 0,
	CMD_PLAYER_LEFT = 5,
	CMD_PLAYER_RIGHT = 6,
	CMD_PLAYER_DOWN = 7,
	CMD_PLAYER_UP = 8,
	CMD_SHOOT = 16,
	CMD_SCORES = 32
} Command;

#endif