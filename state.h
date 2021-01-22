#ifndef STATE_H
#define STATE_H

extern bool gameover;
extern int fraglimit;
extern void restartGame();
extern void stateFrame();
extern void gameOver();
extern void startMatch();
extern bool practice;
extern const int PRACTICE_WAIT;
extern bool usePlayer;
extern int currentQuadrant;
extern bool inGame;
extern void startIntro();
extern bool introReady;

#endif