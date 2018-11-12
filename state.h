#ifndef STATE_H
#define STATE_H

extern bool gameover;
extern int fraglimit;
extern void restartGame();
extern void stateFrame();
extern void gameOver();
extern void startGame();
extern bool practice;
extern const int PRACTICE_WAIT;
extern bool usePlayer;

#endif