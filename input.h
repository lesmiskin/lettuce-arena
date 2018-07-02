#ifndef INPUT_H
#define INPUT_H

#include "common.h"

extern void pollInput(void);
extern void processSystemCommands(void);
extern bool checkCommand(int commandFlag);

typedef enum {
    CMD_QUIT = 0,
	CMD_PLAYER_LEFT = 5,
	CMD_PLAYER_RIGHT = 6,
	CMD_PLAYER_DOWN = 7,
	CMD_PLAYER_UP = 8
} Command;

#endif