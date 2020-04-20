#include <stdio.h>

#include "graphics.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

void clearDisplay(){
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
	printf("Clear Display\n");
}

int initDisplay(){

	if(SDL_Init(SDL_INIT_VIDEO) != 0){
		fprintf(stderr, "SDL Init Video failed\n");
		return -1;
	}

	window = SDL_CreateWindow("CHIP-8", 
							  SDL_WINDOWPOS_CENTERED,
							  SDL_WINDOWPOS_CENTERED,
							  SCREEN_WIDTH, SCREEN_HEIGHT,
							  SDL_WINDOW_SHOWN);
	
	if(window == NULL){
		fprintf(stderr, "FAILED TO CREATE WINODW");
		return -1;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		
	return 0;

}

void destroyDisplay(){
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void renderScreen(uint8_t *gfx){
	
	for(uint8_t y = 0; y < 32; y++){
		for(uint8_t x = 0; x < 64; x++){
			SDL_Rect rect;
			rect.x = x*SCALE;
			rect.y = y*SCALE;
			rect.w = SCALE;
			rect.h = SCALE;
			if(gfx[x + 64*y] != 0) SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			else SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderFillRect(renderer, &rect);
		}
	}

	SDL_RenderPresent(renderer);
}











