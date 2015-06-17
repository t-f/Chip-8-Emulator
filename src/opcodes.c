#include <stdio.h>
#include <stdlib.h> // rand(), srand()
#include <time.h> // time()
#include "include.h"

#define MAX_ROMSIZE 0xCA0
#define VRAM 0xF00

void print_opcode();
void print_opcode_description();
int return_opcode();


int i, j;

extern unsigned short opcode;
extern unsigned char memory[4096];
extern unsigned char V[16];
extern unsigned short I;
extern unsigned short PC;
extern unsigned char framebuffer[64*32];
extern unsigned short stack[16];
extern unsigned short sp;

extern unsigned char delay_timer;
extern unsigned char sound_timer;

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
	case _0NNN:
		printf("NOT IMPLEMENTED\n");
	case _00EE:			// ret
		printf("RET\n");
		printf("PC: %04X\n", PC);
		printf("to\n");
		PC = stack[sp];
		sp--;
		printf("PC: %04X\n", PC);
		PC += 2;
		opcode = memory[PC] << 8 | memory[PC + 1];
		break;

	case _1NNN:
		N = (opcode & 0x0FFF);
		printf("JMP %03X\n", N);
		PC = N;
		opcode = memory[PC] << 8 | memory[PC + 1];
		break;

	case _2NNN:
		N = (opcode & 0x0FFF);
		printf("CALL %03X\n", N);
		sp++;
		printf("PC: %04X, stack[%01X]: %04X\n", PC, sp, stack[sp]);
		printf("to\n");
		stack[sp] = PC;
		PC = N;
		printf("PC: %04X, stack[%01X]: %04X\n", PC, sp, stack[sp]);
		PC -= 2;
		PC += 2;
		opcode = memory[PC] << 8 | memory[PC + 1];
		break;

	case _3XNN:
		X = (opcode & 0x0F00) >> 8;
		N = (opcode & 0x00FF);
		printf("skip next if V[%01X] == %02X\n", X, N);
		printf("V[%01X] = 0x%02X (%d)\n", X, V[X], V[X]);
		printf("PC: %04X\n", PC);
		printf("to\n");
		if (V[X] == N) {
			PC += 2;
		}
		printf("PC: %04X\n", PC);
		PC += 2;
		opcode = memory[PC] << 8 | memory[PC + 1];
		break;

	case _4XNN:
		X = (opcode & 0x0F00) >> 8;
		N = (opcode & 0x00FF);
		printf("skip next if V[%01X] != %02X\n", X, N);
		printf("V[%01X] = 0x%02X (%d)\n", X, V[X], V[X]);
		printf("PC: %04X\n", PC);
		printf("to\n");
		if (V[X] != N) {
			PC += 2;
		}
		printf("PC: %04X\n", PC);
		PC += 2;
		opcode = memory[PC] << 8 | memory[PC + 1];
		break;

	case _5XY0:
		X = (opcode & 0x0F00) >> 8;
		Y = (opcode & 0x00F0) >> 4;
		printf("skip next if V[%01X] == V[%01X]\n", X, Y);
		printf("V[%01X] = 0x%02X (%d)\n", X, V[X], V[X]);
		printf("V[%01X] = 0x%02X (%d)\n", Y, V[Y], V[Y]);
		printf("PC: %04X\n", PC);
		printf("to\n");
		if (V[X] == V[Y]) {
			PC += 2;
		}
		printf("PC: %04X\n", PC);
		PC += 2;
		opcode = memory[PC] << 8 | memory[PC + 1];
		break;

	case _6XNN:
		X = (opcode & 0x0F00) >> 8;
		N = (opcode & 0x00FF);
		printf("V[%01X] = 0x%02X (%d)\n", X, V[X], V[X]);
		printf("to\n");
		V[X] = N;
		printf("V[%01X] = 0x%02X (%d)\n", X, V[X], V[X]);
		PC += 2;
		opcode = memory[PC] << 8 | memory[PC + 1];
		break;

	case _7XNN:
		X = (opcode & 0x0F00) >> 8;
		N = (opcode & 0x00FF);
		printf("V[X] = V[X] + N\n");
		printf("V[%01X] = 0x%02X (%d)\n", X, V[X], V[X]);
		printf("to\n");
		V[X] += N;
		printf("V[%01X] = 0x%02X (%d)\n", X, V[X], V[X]);
		PC += 2;
		opcode = memory[PC] << 8 | memory[PC + 1];
		break;

	case _8XY0:
		X = (opcode & 0x0F00) >> 8;
		Y = (opcode & 0x00F0) >> 4;
		printf("sets V[%01X] = V[%01X]\n", X, Y);
		printf("V[%01X] = 0x%02X (%d)\n", X, V[X], V[X]);
		printf("V[%01X] = 0x%02X (%d)\n", Y, V[Y], V[Y]);
		printf("to\n");
		V[X] = V[Y];
		printf("V[%01X] = 0x%02X (%d)\n", X, V[X], V[X]);
		PC += 2;
		opcode = memory[PC] << 8 | memory[PC + 1];
		break;

	case _8XY1:
		X = (opcode & 0x0F00) >> 8;
		Y = (opcode & 0x00F0) >> 4;
		printf("sets V[%01X] = V[%01X] OR V[%01X]\n", X, X, Y);
		printf("V[%01X] = 0x%02X (%d)\n", X, V[X], V[X]);
		printf("V[%01X] = 0x%02X (%d)\n", Y, V[Y], V[Y]);
		printf("to\n");
		V[X] = (V[X] | V[Y]);
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

	case _CXNN:
		X = (opcode & 0x0F00) >> 8;
		printf("random value to V[%01X]\n", X);
		N = (opcode & 0x00FF);
		printf("V[%01X] = 0x%02X (%d)\n", X, V[X], V[X]);
		printf("to\n");
		V[X] = (rand() % 256) ^ N;
		printf("V[%01X] = 0x%02X (%d)\n", X, V[X], V[X]);
		PC += 2;
		opcode = memory[PC] << 8 | memory[PC + 1];
		break;

	case _DXYN:
		X = (opcode & 0x0F00) >> 8;
		Y = (opcode & 0x00F0) >> 4;
		N = (opcode & 0x000F);
		V[0xF] = 0;

		X = V[X];
		Y = V[Y];

		// this code writes the sprite on memory[i](with height N) on the memory[VRAM] area
		// pending code when the sprite goes off screen
		printf("X: %02X, Y: %02X\n", X, Y);
		for (i = 0; i < N; i++) {
			for (j = 0; j < 8; j++) {
				if (memory[I+i] & (0x80 >> j)) {
					if ((memory[VRAM + 8*(int)(Y+i)+(int)((X+j)/8)] & ((0x80 >> (X+j) % 8))) == 1)
						V[0xF] = 1;
					memory[VRAM + 8*(int)(Y+i)+(int)((X+j)/8)] ^= ((0x80 >> (X+j) % 8));
				}
			}
			printf("%02X\n", memory[I]);
		}
		printf("V[F] = 0x%02X\n", V[0xF]);
		printf("framebuffer written\n");
		PC += 2;
		opcode = memory[PC] << 8 | memory[PC + 1];
		break;

	case _FX07:
		X = (opcode & 0x0F00) >> 8;
		printf("V[%01X] = %02X, Delay timer = %02X\n", X, V[X], delay_timer);
		printf("to\n");
		V[X] = delay_timer;
		printf("V[%01X] = %02X, Delay timer = %02X\n", X, V[X], delay_timer);
		PC += 2;
		opcode = memory[PC] << 8 | memory[PC + 1];
		break;

	case _FX15:
		X = (opcode & 0x0F00) >> 8;
		printf("Delay timer = %d, V[%01X] = %02X\n", delay_timer, X, V[X]);
		printf("to\n");
		delay_timer = V[X];
		printf("Delay timer = %d, V[%01X] = %02X\n", delay_timer, X, V[X]);
		PC += 2;
		opcode = memory[PC] << 8 | memory[PC + 1];
		break;

	case _FX29:
		#define FONT_SIZE 5
		X = (opcode & 0x0F00) >> 8;
		printf("I: %04X\n", I);
		printf("to\n");
		I = FONT_SIZE * (V[X] & 0x0F);
		printf("I: %04X\n", I);
		PC += 2;
		opcode = memory[PC] << 8 | memory[PC + 1];
		break;

	case _FX33:
		X = (opcode & 0x0F00) >> 8;
		printf("memory[%04X]: %02X\n", I, memory[I]);
		printf("memory[%04X]: %02X\n", I+1, memory[I+1]);
		printf("memory[%04X]: %02X\n", I+2, memory[I+2]);
		printf("V[%01X]: %02X\n", X, V[X]);
		printf("to\n");
		memory[I] = V[X] / 100;
  		memory[I + 1] = (V[X] / 10) % 10;
  		memory[I + 2] = (V[X] % 100) % 10;
		printf("memory[%04X]: %02X\n", I, memory[I]);
		printf("memory[%04X]: %02X\n", I+1, memory[I+1]);
		printf("memory[%04X]: %02X\n", I+2, memory[I+2]);
		printf("V[%01X]: %02X\n", X, V[X]);
		PC += 2;
		opcode = memory[PC] << 8 | memory[PC + 1];
		break;

	case _FX65:
		X = (opcode & 0x0F00) >> 8;
		printf("I: %04X\n", I);
		for (i = 0; i <= X; i++)
			printf("V[%01X]: %02X\n", i, V[i]);
		printf("to\n");
		for (i = 0; i <= X; i++) {
			V[i] = memory[I+i];
			printf("V[%01X]: %02X\n", i, V[i]);
		}
		PC += 2;
		opcode = memory[PC] << 8 | memory[PC + 1];
		break;
	}
	//PC += 2;
	//opcode = memory[PC] << 8 | memory[PC + 1];
}
