#include <stdio.h>
#include "include.h"

void print_opcode();
void print_opcode_description();
int return_opcode();

extern unsigned short opcode;
extern unsigned char memory[4096];
extern unsigned char V[16];
extern unsigned short I;
extern unsigned short PC;

void print_opcode() {
	printf("%s", opcode_array[return_opcode()]);
}

void print_opcode_description() {
	printf("%s", opcode_description_array[return_opcode()]);
}

int return_opcode() {
	switch(opcode & 0xF000) {
	case 0x0000:
		if (opcode == 0x00E0)
			return _00E0;
		else if (opcode == 0x00EE) {
			return _00EE;
		}
		else
			return _0NNN;
	case 0x1000:
		return _1NNN;
	case 0x2000:
		return _2NNN;
	case 0x3000:
		return _3XNN;
	case 0x4000:
		return _4XNN;
	case 0x5000:
		return _5XY0;
	case 0x6000:
		return _6XNN;
	case 0x7000:
		return _7XNN;
	case 0x8000:
		if ((opcode & 0xF00F) == 0x8000)
			return _8XY0;
		else if ((opcode & 0xF00F) == 0x8001)
			return _8XY1;
		else if ((opcode & 0xF00F) == 0x8002)
			return _8XY2;
		else if ((opcode & 0xF00F) == 0x8003)
			return _8XY3;
		else if ((opcode & 0xF00F) == 0x8004)
			return _8XY4;
		else if ((opcode & 0xF00F) == 0x8005)
			return _8XY5;
		else if ((opcode & 0xF00F) == 0x8006)
			return _8XY6;
		else if ((opcode & 0xF00F) == 0x8007)
			return _8XY7;
		else if ((opcode & 0xF00F) == 0x800E)
			return _8XYE;
	case 0x9000:
		return _9XY0;
	case 0xA000:
		return _ANNN;
	case 0xB000:
		return _BNNN;
	case 0xC000:
		return _CXNN;
	case 0xD000:
		return _DXYN;
	case 0xE000:
		if ((opcode & 0xF0FF) == 0xE09E)
			return _EX9E;
		else if ((opcode & 0xF0FF) == 0xE0A1)
			return _EXA1;
	case 0xF000:
		if ((opcode & 0xF0FF) == 0xF007)
			return _FX07;
		else if((opcode & 0xF0FF) == 0xF00A)
			return _FX0A;
		else if((opcode & 0xF0FF) == 0xF015)
			return _FX15;
		else if((opcode & 0xF0FF) == 0xF018)
			return _FX18;
		else if((opcode & 0xF0FF) == 0xF01E)
			return _FX1E;
		else if((opcode & 0xF0FF) == 0xF029)
			return _FX29;
		else if((opcode & 0xF0FF) == 0xF033)
			return _FX33;
		else if((opcode & 0xF0FF) == 0xF055)
			return _FX55;
		else if((opcode & 0xF0FF) == 0xF065)
			return _FX65;
	}
	return 0;
}

void exec_opcode() {
	unsigned int X, Y, N;
	switch(return_opcode()) {
	case _6XNN:
		//printf("0x6000\n");
		X = (opcode & 0x0F00) >> 8;
		//printf("X: %X\n", X);
		N = (opcode & 0x00FF);
		//printf("N: %X\n", N);
		printf("V[%01X] = 0x%02X (%d)\n", X, V[X], V[X]);
		printf("to\n");
		V[X] = N;
		printf("V[%01X] = 0x%02X (%d)\n", X, V[X], V[X]);
		PC += 2;
		opcode = memory[PC] << 8 | memory[PC + 1];
		break;
	case _ANNN:
		N = (opcode & 0x0FFF);
		printf("I = 0x%04X (%d)\n", I, I);
		printf("to\n");
		I = N;
		printf("I = 0x%04X (%d)\n", I, I);
		PC += 2;
		opcode = memory[PC] << 8 | memory[PC + 1];
		break;
	}
}
