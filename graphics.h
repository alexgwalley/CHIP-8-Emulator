#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_ttf.h>

#define SCALE 10

#define SCREEN_WIDTH 64*SCALE
#define SCREEN_HEIGHT 32*SCALE

extern SDL_Window *window;
extern SDL_Renderer *renderer;

extern TTF_Font *font;

extern void clearDisplay();

extern int initDisplay();

extern void destroyDisplay();

extern void renderScreen(uint8_t *gfx);

#endif /* GRAPHICS_H */
