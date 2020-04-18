#include <stdio.h>

#include "graphics.h"

SDL_Window *window = NULL;

int initDisplay(){

	SDL_Renderer *renderer = NULL;

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

	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	SDL_Event e;
	int quit = 0;
	while (quit == 0){
	    while (SDL_PollEvent(&e)){
	        if (e.type == SDL_QUIT){
	            quit = 1;
	        }
	        if (e.type == SDL_KEYDOWN){
	            quit = 1;
	        }
	        if (e.type == SDL_MOUSEBUTTONDOWN){
	            quit = 1;
	        }
	    }
	}	

	SDL_DestroyRenderer(renderer);

	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;

}

