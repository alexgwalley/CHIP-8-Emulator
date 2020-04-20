
#include "input.h"

// Input
// CHIP-8 uses a hex based keyboard with 16 keys
uint8_t keys[16];

void updateKeys(SDL_KeyboardEvent *key){

	uint8_t t;
	//printf("Scancode: 0x%02X\n", key->keysym.scancode);

	t = (key->type == SDL_KEYUP) ? 0 : 1;
	switch(key->keysym.scancode){
		case 0x1E: 	// 1
			keys[0x0] = t;
			break;
		case 0x1F:	// 2
			keys[0x1] = t;
			break;
		case 0x20:	// 3
			keys[0x2] = t;
			break;
		case 0x21:	// 4
			keys[0x3] = t;
			break;
		case 0x14:	// Q
			keys[0x4] = t;
			break;
		case 0x1A:	// W
			keys[0x5] = t;
			break;
		case 0x08: 	// E
			keys[0x6] = t;
			break;
		case 0x15: 	// R
			keys[0x7] = t;
			break;
		case 0x04: 	// A
			keys[0x8] = t;
			break;
		case 0x16: 	// S
			keys[0x9] = t;
			break;
		case 0x07: 	// D
			keys[0xA] = t;
			break;
		case 0x09:	// F
			keys[0xB] = t;
			break;
		case 0x1D:	// Z
			keys[0xC] = t;
			break;
		case 0x1B: 	// X
			keys[0xD] = t;
			break;
		case 0x06:	// C
			keys[0xE] = t;
			break;
		case 0x19: 	// V
			keys[0xF] = t;
			break;
		default:
			break;
	}

}