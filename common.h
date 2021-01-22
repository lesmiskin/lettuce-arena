#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include "mysdl.h"

extern bool CHEAT_GIVE_MACH;
extern bool CHEAT_GIVE_ROCK;
extern bool CHEAT_GOD;
extern bool DEBUG_SKIP_INTRO;
extern bool DEBUG_NO_ENEMIES;
extern bool PLAYER_INDICATOR;
extern bool DEBUG_STILL_ENEMIES;
extern bool DEBUG_ONE_SHOT_KILLS;

typedef struct {
    double x, y;
} Coord;

typedef enum {
	MODE_GAME,
	MODE_TITLE
} GameMode;

typedef struct {
	double x, y;
	int width, height;
} Rect;

extern double getDistance(Coord a, Coord b);
extern double radToDeg(double radians);
extern double degToRad(double degrees);
extern double calcDistance(Coord a, Coord b);
extern Rect makeRect(double x, double y, double width, double height);
extern bool inBounds(Coord point, Rect area);
extern Rect makeBounds(Coord origin, double width, double height);
extern Rect makeSquareBounds(Coord origin, double size);
extern GameMode currentMode;
extern void changeMode(GameMode newMode);
extern SDL_Window *window;
extern bool running;
extern Coord makeCoord(double x, double y);
extern Coord mergeCoord(Coord original, Coord derive);
extern Coord deriveCoord(Coord original, double xOffset, double yOffset);
extern Coord zeroCoord();
extern bool timer(long *lastTime, double hertz);
extern bool isDue(long now, long lastTime, double hertz);
extern void fatalError(const char *title, const char *message);
extern void quit(void);
extern char *combineStrings(const char *a, const char *b);
extern bool fileExists(const char *path);
extern int randomMq(int min, int max);
extern double sineInc(double offset, double *sineInc, double speed, double magnitude);
extern double cosInc(double offset, double *sineInc, double speed, double magnitude);
extern double getAngle(Coord a, Coord b);
extern Coord getAngleStep(double angle, double speed, bool negativeMagic);
extern Coord getStep(Coord a, Coord b, double speed, bool negativeMagic);
extern bool chance(int probability);
extern bool onScreen(Coord coord, double threshold);
extern Coord extendOnAngle(Coord origin, double radians, int distance);

#endif