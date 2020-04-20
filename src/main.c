#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "graphics.h"
#include "input.h"

#define print_byte(BYTE) for(int i = 8; i  >= 0; i--) printf("%d", (BYTE>>i)&1);

// ========== SETUP ==========

//	Memory
/* 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
 * 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
 * 0x200-0xFFF - Program ROM and work RAM
 */
#define MEM_SIZE 0x1000
#define PGM_ROM_START 0x200
uint8_t memory[MEM_SIZE];

// CPU has registers V0-VF, VF = flags register
uint8_t V[16]; 	// Registers
uint16_t opcode;

uint16_t I; 	// Index register
uint16_t PC; 	// Program counter

// Graphics
#define FONT_OFF 0x50
uint8_t gfx[ 64 * 32 ];
uint8_t draw_flag = 0;


// Timings / Interrupts (60Hz)
uint64_t delay_timer;
uint64_t sound_timer;

// Interpreter Stack
/* Specifications don't call for stack, 
 * but the interpreter needs one to keep track
 * of subroutine calls. It can only go 16
 * levels deep
 */
#define STACK_SIZE 200
uint16_t stack[STACK_SIZE];
uint16_t SP; 		// Stack pointer

// Debugging
int debug = 0;


// ========== FONTSET ==========
uint8_t chip8_fontset[80] =
{ 
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

// ========== CODE ==========

/* Opens .ch8 file (ROM) 
 * and loads it into memory.
 * returns the number of bytes read
 */	
int loadROM(char *filename){
	FILE *fp;

	#if _WIN64
	fp = fopen_s(filename, "rb");
	#else
	fp = fopen(filename, "rb");
	#endif
	if(!fp) return -1;

	return fread(memory+PGM_ROM_START, 1, MEM_SIZE-PGM_ROM_START, fp);
}

/* Loads the CHIP-8 font
 * set into memory starting at
 * loc 0x50
 */
void loadFontSet(){
	for(int i = 0; i < 80; i++){
		memory[i + FONT_OFF] = chip8_fontset[i];
	}
}

/*  Dumps the values on the stack
 *	to standard output, meant
 *  for debugging
 */
void printStack(){
	printf("Stack Pointer: 0x%02X\n", SP);
	for(int i = 0; i < STACK_SIZE && stack[i] != 0; i++)
		printf("(%d) 0x%02X\n", i, stack[i]);
}

/*  Sets delay_timer to value
 *	in register intexed by reg
 */
void setDelayTimer(int reg){
	/*
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	delay_timer = now.tv_nsec + (uint64_t)(V[reg])*(1e9/60);
	printf("\nDelay Offset: %fms\n", (uint64_t)(V[reg])*(1e9/60)/1e6);
	*/
	delay_timer = V[reg];
}

/*  Returns the delay_timer value
 *	If negative, returns zero
 */
uint8_t getDelayTimer(){
	/*
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);

	if (now.tv_nsec < delay_timer) { // Still time left
		return (uint8_t)((delay_timer - now.tv_nsec) * (60 / 1e9));
	}

	return 0;
	*/
	return (delay_timer > 0 ) ? delay_timer : 0;
}

/* Sets the starting state for:
 * - CPU (registers, PC, SP).
 * - Graphics
 * - Memory
 * - Stack
 */
void initialize(){

	PC = 0x200;
	opcode = 0;
	I = 0;
	SP = 0;

	draw_flag = 0;
	
	initDisplay();						// Clear Display

	memset(stack, 0, sizeof(stack));	// Clear Stack
	memset(V, 0x00, sizeof(V));			// Clear registers
	memset(memory, 0x00, MEM_SIZE); 	// Clear memory

	loadFontSet();

	// Reset Timers
	delay_timer = 0;
	sound_timer = 0;

}

/*	The meat of the program. 
 *  Handles Fetching, decoding, 
 *  and executing the next opcode
 *  at PC.
 */
int emulateCycle(){


	uint16_t x, y;
	uint8_t n;
	// Get opcode
	opcode = memory[PC] << 8 | memory[PC + 1];
	if(debug) printf(" oc: 0x%04X ", opcode);

	// Decode and Excecute opcode
	switch(opcode & 0xF000){

		case 0x0000:
			switch(opcode & 0x0FFF){
				case 0x00E0: // Clear the Screen
					memset(gfx, 0, sizeof(gfx));
					renderScreen(gfx);
					PC += 2;
					if(debug) printf("CLS\n");
					break;
				case 0x00EE:
					PC = stack[--SP];
					if(debug) printf("RET\n");
					break;
				default: // SYS addr (jump to a machine code routine at nnn)
					PC = opcode & 0x0FFF;
					if(debug) printf("SYS 0x%03x\n", opcode & 0x0FFF);
					break;
			}
			break;

		case 0x1000: //Jump to 0x0nnn
			PC = opcode & 0x0FFF;
			if(debug) printf("JP 0x%03X\n", PC);
			break;
		case 0x2000: // Call subroutine at 0x0nnn
			stack[SP++] = PC+2;
			if(SP > STACK_SIZE) {  printStack(); printf("Stack overflow!\n"); return -1; }
			PC = (opcode & 0x0FFF);
			if(debug) printf("CALL  0x%03X\n", PC);
			break;
		case 0x3000: // Skip next instruction if Vx = kk
			if(V[(opcode & 0x0F00)>>8] == (opcode & 0x00FF)) PC += 4;
			else PC += 2;
			if(debug) printf("SE V%01X, 0x%02X\n", (opcode & 0x0F00) >> 8, (opcode & 0x00FF));
			break;
		case 0x4000:	// Skip next instruction if Vx != kk
			if(V[(opcode & 0x0F00)>>8] != (opcode & 0x00FF)) PC += 4;
			else PC += 2;
			if(debug) printf("SNE V%01X, 0x%02X\n", (opcode & 0x0F00) >> 8, (opcode & 0x00FF));
			break;
		case 0x5000:    // Skip next instruction if Vx = Vy	
			if(V[(opcode & 0x0F00)>>8] == V[(opcode & 0x00F0)>>4]) PC += 4;
			else PC += 2;
			if(debug) printf("SE V%X, V%x", opcode&0x0F00>>8, opcode&0x00F00>>4);
			break;
		case 0x6000: 	// 0x6xkk Set Vx to kk
			V[(opcode & 0x0F00)>>8] = (opcode & 0x00FF);
			PC += 2;
			if(debug) printf("LD V%x, 0x%02x\n", (opcode & 0x0F00) >> 8, opcode & 0x00ff);
			break;
		case 0x7000:    // ADD Vx, BYTE
			V[(opcode & 0x0F00)>>8] += (opcode & 0x00FF);
			PC += 2;
			if(debug) printf("ADD V%X, 0x%02x\n", (opcode & 0x0F00)>>8, opcode&0x00FF);
			break;
		case 0x8000:
			switch(opcode & 0x000F){
				case 0x0000: // Set Vx = Vy
					V[(opcode & 0x0F00)>>8] = V[(opcode & 0x00F0)>>4];
					PC += 2;
					if(debug) printf("LD V%X, V%X\n", (opcode & 0x0F00)>>8, (opcode & 0x00F0)>>4);
					break;
				case 0x0001: // Vx = Vx OR Vy
					V[(opcode & 0x0F00)>>8] |= V[(opcode & 0x00F0)>>4];
					PC += 2;
					if(debug) printf("OR V%X, V%X\n", (opcode & 0x0F00)>>8, (opcode & 0x00F0)>>4);
					break;
				case 0x0002: // Vx = Vx AND Vy
					V[(opcode & 0x0F00)>>8] &= V[(opcode & 0x00F0)>>4];
					PC += 2;
					if(debug) printf("AND V%X, V%X\n", (opcode & 0x0F00)>>8, (opcode & 0x00F0)>>4);
					break;
				case 0x0003: // Vx = Vx XOR Vy
					V[(opcode & 0x0F00)>>8] ^= V[(opcode & 0x00F0)>>4];
					PC += 2;
					if(debug) printf("XOR V%X, V%X\n", (opcode & 0x0F00)>>8, (opcode & 0x00F0)>>4);
					break;
				case 0x0004: // Vx = Vx + Vy, set VF = carry
					if(V[(opcode & 0x0F00)>>8] + V[(opcode & 0x00F0)>>4] > 0xFF) V[0xF] = 1;
					else V[0xF] = 0;
					V[(opcode & 0x0F00)>>8] = ( (V[(opcode & 0x00F0)>>4] + V[(opcode & 0x0F00)>>8]) & 0x00FF );
					PC += 2;
					if(debug) printf("ADD V%X, V%X\n", (opcode & 0x0F00)>>8, (opcode & 0x00F0)>>4);
					break;
				case 0x0005: // Vx = Vx-Vy
					V[(opcode & 0x0F00)>>8] = V[(opcode & 0x0F00)>>8] - V[(opcode & 0x00F0)>>4];
					PC += 2;
					if(debug) printf("SUB V%X, V%X", opcode&0x0F00>>8, opcode&0x00F0);
					break;	
				case 0x0006: // Vx = Vx SHR 1
					if(V[(opcode&0x0F00)>>8] & 1) V[0xF] = 1;
					else V[0xF] = 0;
					V[(opcode&0x0F00)>>8] >>= 2;
					PC += 2;
					if(debug) printf("SHR V%x {, V%x}\n", opcode&0x0F00>>8, opcode&0x00F0>>4);
					break;
				case 0x0007: // SUBN Vx, Vy set Vx = Vy - Vx, VF = NOT borrow
					if(V[(opcode&0x00F0)>>4] > V[(opcode&0x0F00)>>8]) V[0xF] = 1;
					else V[0xF] = 0;
					V[opcode&0x0F00>>8] -= V[(opcode&0x00F0)>>4];
					PC += 2;
					if(debug) printf("SUBN V%x, V%x\n", (opcode & 0x0F00)>>8, (opcode&0x00F0)>>4);
					break;
				case 0x000E: // SHL Vx {, Vy}
					if(V[(opcode&0x0F00)>>8]&0x80) V[0xF] = 1;
					else V[0xF] = 0;
					V[(opcode & 0x0F00)>>8] <<= 2;
					PC += 2;
					if(debug) printf("SHL V%x {, V%x}\n", (opcode & 0x0F00)>>8, (opcode & 0x00F0)>>4);
					break;

				default:
					printf("Unknown opcode: 0x%X\n", opcode);
					return -1;
			}
			break;
		case 0x9000:    // Skip next instruction if Vx != Vy
			if(V[(opcode & 0x0F00)>>8] != V[(opcode & 0x00F0)>>4]) PC += 4;
			else PC += 2;
			if(debug) printf("SNE V%X, V%X", (opcode & 0x0F00)>>8, (opcode & 0x00F0)>>4);
			break;
		case 0xA000:	// Set I to nnn
			I = opcode & 0x0FFF;
			PC += 2;
			if(debug) printf("LD I, %03X\n", opcode & 0x0FFF);
			break;
		case 0xB000:    // Jump to location nnn + V0
			PC = V[0] + (opcode&0x0FFF);
			if(debug) printf("JP V0, %03X\n", opcode & 0x0FFF);
		case 0xC000: 	// 0xCxkk Set Vx = random byte AND kk
			V[(opcode & 0x0F00)>>8] = rand() & (opcode & 0x00FF);
			PC += 2;	
			if(debug) printf("RND V%X, %02X\n", (opcode & 0x0F00)>>8, (opcode & 0x00FF));	
			break;
		case 0xD000: // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
			// 0xDxyn
			x = V[(opcode & 0x0F00)>>8];
			y = V[(opcode & 0x00F0)>>4];
			uint16_t height = opcode & 0x000F;
			uint16_t pixel;

			V[0xF] = 0;
			// For every pixel in the sprite, write the pixel to GFX
			for(int yline = 0; yline < height; yline++){
				pixel = memory[I + yline];
				for(int xline = 0; xline < 8; xline++){
					if((pixel & (0x80 >> xline)) != 0){	// If the sprite from memory has pixel set
						if(gfx[(x + xline) + ((y+yline)*64)] > 0)
							V[0xF] = 1; // Collision Dectected!
						gfx[(x + xline) + ((y + yline) * 64)] ^= 1;
					}
				}
			}
			draw_flag = 1;
			PC += 2;
			if(debug) printf("DRW V%X(0x%02X), V%X(0x%02X), %X\n", 
							(opcode&0x0F00)>>8, V[(opcode&0x0F00)>>8],
							(opcode&0x00F0)>>4, V[(opcode&0x00F0)>>4],
							(opcode&0x000F));	
			break;
		case 0xE000:
			switch(opcode & 0x00FF){
				case 0x009E:	// Skip next instruction if key with value of Vx is pressed
					if(keys[ V[(opcode & 0x0F00)>>8] ] != 0) PC += 4;
					else PC += 2;
					if(debug) printf("SKP V%X (0x%02X)\n", (opcode&0x0F00)>>8, V[(opcode & 0x0F00)>>8]);
				break;
				case 0x00A1:	// Skip next instruction if key with value of Vx is not pressed
					if(keys[ V[(opcode & 0x0F00)>>8] ] == 0) PC += 4;
					else PC += 2;
					if(debug) printf("SKNP V%X (0x%02X)\n", (opcode&0x0F00)>>8, V[(opcode & 0x0F00)>>8]);
				break;
			}
			break;
		case 0xF000:
			switch(opcode & 0x00FF){
				case 0x0007:  // Set Vx to delay timer
					V[(opcode&0x0F00)>>8] = getDelayTimer();
					PC += 2;
					if(debug) printf("LD V%X, DT\n", (opcode&0x0F00)>>8);
					break;
				case 0x0015: // Set delay_timer to Vx
					setDelayTimer((opcode & 0x0F00)>>8);
					PC += 2;
					if(debug) printf("LD DT, V%X\n", (opcode&0x0F00)>>8);
					break;
				case 0x0018: // Set sound_timer to Vx
					sound_timer = V[(opcode & 0x0F00)>>8];
					PC += 2;
					if(debug) printf("LD ST, V%X\n", (opcode&0x0F00)>>8);
					break;
				case 0x001E: // Set I to I + Vx
					I += V[(opcode & 0x0F00)];
					PC += 2;
					if(debug) printf("ADD I, V%x\n", (opcode&0x0F00)>>8);
					break;
				case 0x0029: // Set I = location of sprite for digit Vx
					I = FONT_OFF + ( V[ (opcode & 0x0F00)>>8 ] * 5 );
					PC += 2;
					if(debug) printf("LD F, V%X\n", (opcode&0x0F00)>>8);
					break;
				case 0x0033: // Store BCD representation of Vx in memory locations I, I+1, I+2
					n = V[(opcode&0x0F00)>>8];
					memory[I]   = (n/100);
					memory[I+1] = (n/10)%10;
					memory[I+2] = n%10;
					PC += 2;
					if(debug) printf("LD B, V%x", (opcode&0x0F00)>>8);
					break;
				case 0x0055: // Store Registers V0 through Vx in memory starting at I
					for(int i = 0; i <= (opcode&0x0F00)>>8; i++)
						memory[I+i] = V[i];
					PC += 2;
					if(debug) printf("LD [I], V%X\n", (opcode&0x0F00)>>8);
					break;	
				case 0x0065: // Read Registers V0 through Vx from memory starting at I
					for(int i = 0; i <= (opcode&0x0F00)>>8; i++)
						V[i] = memory[I+i];
					PC += 2;
					if(debug) printf("LD  V%X, [I]\n", (opcode&0x0F00)>>8);
					break;
				default:
					printf("Unknown opcode: 0x%X\n", opcode);
					return -1;
			}
			break;
		default:
			printf("Unknown opcode: 0x%X\n", opcode);
			return -1;
	
	}

	// Decrement timers
	if(delay_timer) --delay_timer;
	if(sound_timer > 0) {	// TODO: Make actual tone
		printf("BEEP\n");
		//if(sound_timer == 1) printf("BEEP\n");
		--sound_timer;
	} 

	return 0;
}

void delay(int number_of_seconds){ // Maybe better option with interrupt? Does C do that? 
	int milli_seconds = 1000 * number_of_seconds;
	clock_t start_time = clock();
	while (clock() < start_time + milli_seconds) ;
}

int main(int argc, char *argv[]){

	char *game_name;

	if(argc > 2) {
		fprintf(stderr, "Usage: %%PROGRAM_NAME%% [GAME_NAME]\n");
		exit(1);
	}else if(argc == 2){
		game_name = argv[1];
	}else{
		printf("Playing default game: pong\n");
		game_name = "ZeroPong";
	}

	SDL_Event e;
	int quit;
	int pause;

	//Initialize CHIP-8
	printf("Initializing CPU...\n");
	initialize();

	//Load ROM
	char path_name[256];
	sprintf(path_name, "./GAMES/%s.ch8", game_name);

	printf("Loading ROM...\n");
	if(loadROM(path_name) <= 0) { fprintf(stderr, "Failed to open ROM."); exit(1); }
	printf("ROM loaded successfully!\n");


	quit = 0;
	pause = 0;
	while (quit == 0){ // Slow! Maybe better way
	    while (SDL_PollEvent(&e)){
	        if (e.type == SDL_QUIT) quit = 1;
	        
	        if(e.key.keysym.scancode == 0x13 && e.key.type == SDL_KEYDOWN) pause = (pause) ? 0 : 1;
	        
	        updateKeys(&e.key);
	    }

	    if(debug){
	    	printf("Keys: ");
		    for(int i = 0; i <= 0xF; i++){
		    	printf("%d", (keys[i]> 0) ? 1 : 0);
		    }
		    printf(" ");
		}

	    if(pause) {
	    	SDL_WaitEvent(&e);
	    	if(e.key.keysym.scancode == 0x13 && e.key.type == SDL_KEYDOWN) 
	    		pause = (pause) ? 0 : 1;

	    };

	    // Step Emulator
		if(debug) printf("PC: 0x%03X ", PC);
	    if(emulateCycle() < 0) quit = 1;
	    if(draw_flag) { renderScreen(gfx); draw_flag = 0; }

	    delay(1000/60); // freq = 60Hz
	}

	// Clean up
	destroyDisplay();
}







