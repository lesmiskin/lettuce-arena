#ifndef INPUT_H
#define INPUT_H

#include "common.h"

#define MAX_COMMANDS 10

extern void pollInput(void);
extern void processSystemCommands(void);
extern bool checkCommand(int commandFlag);
extern bool commands[MAX_COMMANDS];

typedef enum {
    CMD_QUIT,
	CMD_PLAYER_LEFT,
	CMD_PLAYER_RIGHT,
	CMD_PLAYER_DOWN,
	CMD_PLAYER_UP,
	CMD_SHOOT,
	CMD_SCORES,
	CMD_RESTART,
	CMD_START
} Command;

#endif
