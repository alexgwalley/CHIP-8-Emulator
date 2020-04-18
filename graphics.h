#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

extern SDL_Window *window;

//void clearDisplay();

extern int initDisplay();

#endif /* GRAPHICS_H */
