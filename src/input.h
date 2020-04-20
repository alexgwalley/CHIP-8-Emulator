#ifndef __INPUT
#define __INPUT

#if _WIN64
#include "SDL.h"
#else
#include <SDL2/SDL.h>
#endif
extern uint8_t keys[16];
extern void updateKeys();

#endif