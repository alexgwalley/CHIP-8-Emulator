#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graphics.h"

// ========== SETUP ==========

//	Memory
/* 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
 * 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
 * 0x200-0xFFF - Program ROM and work RAM
 */
#define MEM_SIZE 0x1000
#define PGM_ROM_START 0x200
uint8_t memory[MEM_SIZE];

// CPU V0-VF, VF = flags register
uint16_t opcode;
uint8_t V[16]; 	// Registers

uint16_t I; 	// Index register
uint16_t PC; 	// Program counter

// Graphics
uint8_t gfx[ 64 * 32 ];


// Timings / Interrupts (60Hz)
uint8_t delay_timer;
uint8_t sound_timer;

// Interpreter Stack
/* Specifications don't call for stack, 
 * but the interpreter needs one to keep track
 * of subroutine calls. It can only go 16
 * levels deep
 */
uint16_t stack[16];
uint16_t SP; 		// Stack pointer

// Input
// CHIP-8 uses a hex based keyboard with 16 keys
uint8_t key[16];	


// ========== CODE ==========

/* Opens .ch8 file (ROM) 
 * and loads it into memory.
 */	
void loadROM(char *filename){

	FILE *fp;
	int num_bytes_read;

	fp = fopen(filename, "rb");
	if(!fp){
		fprintf(stderr, "Failed to open ROM.");
		exit(1);
	}

	num_bytes_read = fread(memory+PGM_ROM_START, 1, 
						   MEM_SIZE-PGM_ROM_START, fp);

	printf("Bytes Loaded: 0x%x (%d)\n", num_bytes_read, num_bytes_read);

	if(num_bytes_read <= 0){
		fprintf(stderr, "Failed to read ROM.");
		exit(1);
	}

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

	// Clear Display
	initDisplay();

	// Clear Stack
	memset(stack, 0, sizeof(stack));
	// Clear registers
	memset(V, 0x00, sizeof(V));
	// Clear memory
	memset(memory, 0x00, MEM_SIZE);

	// Load fontset


	// Reset Timers
	delay_timer = 0;
	sound_timer = 0;

}

void emulateCycle(){

	// Get opcode
	opcode = memory[PC] << 8 | memory[PC + 1];

	// Decode opcode


	// Excecute opcode


	// Update timers
	if(delay_timer > 0) --delay_timer;
	if(sound_timer > 0) {
		if(sound_timer == 0) printf("BEEP\n");
		--sound_timer;
	}


}


int main(){

	//Initialize CHIP-8
	printf("Initializing CPU...\n");
	initialize();

	//Load ROM
	printf("Loading ROM...\n");
	loadROM("/Users/alexwalley/Code/CHIP-8/GAMES/ZeroPong.ch8");
	printf("ROM loaded successfully!\n");

	for(;;){
		emulateCycle();

		// Draw to screen, if flag set

		// Store key press state

	}


}







