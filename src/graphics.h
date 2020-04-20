#ifndef GRAPHICS_H
#define GRAPHICS_H

#if _WIN64
#include "SDL.h"
#include "SDL_video.h"
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#endif

#define SCALE 10

#define SCREEN_WIDTH 64*SCALE
#define SCREEN_HEIGHT 32*SCALE

extern SDL_Window *window;
extern SDL_Renderer *renderer;

extern void clearDisplay();

extern int initDisplay();

extern void destroyDisplay();

extern void renderScreen(uint8_t *gfx);

#endif /* GRAPHICS_H */
