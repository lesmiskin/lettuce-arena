#ifndef HUD_H
#define HUD_H

extern void initHud(void);
extern void writeText(int amount, Coord pos);
extern void writeFont(char *text, Coord pos);
extern void hudRenderFrame(void);
extern void hudGameFrame(void);
extern void hudAnimFrame(void);
extern int fraglimit;
extern void writeFontFull(char *text, Coord pos, bool centerScreen, bool centerText);
extern void writeAmount(int amount, Coord pos);

#endif
