#ifndef RENDERER_H
#define RENDERER_H

#include "mysdl.h"
#include "common.h"

typedef struct {
    SDL_Texture *texture;
    Coord offset;
    Coord size;
    SDL_RendererFlip flip;
} Sprite;

typedef struct {
	int red, green, blue, alpha;
} Colour;

typedef enum {
	COLOURISE_ABSOLUTE = 0,
	COLOURISE_ADDITIVE = 1
} ColourisationMethod;

extern Colour makeColour(int red, int green, int blue, int alpha);
extern SDL_Surface *colouriseSprite(SDL_Surface *original, Colour colour, ColourisationMethod method);
extern int viewOffsetY;

extern SDL_Renderer *renderer;
extern SDL_Texture *renderBuffer;
extern Coord screenBounds;
extern Coord pixelGrid;
extern Coord screenBounds;
extern void initRenderer();
extern void shutdownRenderer();
extern void updateCanvas();
extern Sprite makeSprite(SDL_Texture *texture, Coord offset, SDL_RendererFlip flip);
extern void drawSpriteFull(Sprite sprite, Coord origin, double scalex, double scaley, double angle, bool centered);
extern void drawSprite(Sprite sprite, Coord origin);
extern Coord getTextureSize(SDL_Texture *texture);
extern Sprite makeFlippedSprite(char *textureName, SDL_RendererFlip flip);
extern Sprite makeSimpleSprite(char *textureName);

#endif
