#include <stdio.h> 	// printf(), scanf(), FILE, fopen(), fread()

#define MAX_ROMSIZE 0xCA0
#define VRAM 0xF00

void print_opcode();
void print_opcode_description();
void exec_opcode();

unsigned char chip8_fontset[80] = 
{ 
	0xF0, 0x90, 0x90, 0x90, 0xF0, 	// 0
	0x20, 0x60, 0x20, 0x20, 0x70, 	// 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, 	// 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, 	// 3
	0x90, 0x90, 0xF0, 0x10, 0x10, 	// 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, 	// 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, 	// 6
	0xF0, 0x10, 0x20, 0x40, 0x40, 	// 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, 	// 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, 	// 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, 	// A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, 	// B
	0xF0, 0x80, 0x80, 0x80, 0xF0, 	// C
	0xE0, 0x90, 0x90, 0x90, 0xE0, 	// D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, 	// E
	0xF0, 0x80, 0xF0, 0x80, 0x80 	// F
};

int i, j;
int display_description = 1;

unsigned short opcode;
unsigned char memory[4096];

unsigned char V[16];
unsigned short I;
unsigned short PC; // Program/instruction pointer

unsigned char delay_timer;
unsigned char sound_timer;

unsigned char framebuffer[64*32]; // 2048 pixels

unsigned short stack[16];
unsigned short sp; // stack pointer

unsigned char key[16]; // keyboard current state
unsigned char rom[MAX_ROMSIZE]; // ROM will be loaded here
unsigned int romsize;

int load_rom() {
	FILE* fp = fopen("../ROMs/PONG", "rb");
	if (!fp) {
		fclose(fp);
		return 0;
	}
	int result = fread(rom, 1, MAX_ROMSIZE, fp);
	romsize = ftell(fp);
	if (!result) {
		fclose(fp);
		return 0;
	}
	else {
		fclose(fp);
		return 1;
	}
}

void print_rom() {
	printf("ROM size: %d bytes\n\n", romsize);
	for (i = 0; i < 101; i++) {
		for (j = 0; j < 32; j++) {
			if (32*i+j < romsize) {
				printf("%02X", rom[32*i+j]);
			}
			else {
				i = 101;
				break;
			}
		}
		printf("\n");
	}
}

void chip8_initialize() {
	PC = 0x200;
	I = 0;
	sp = 0;

	// loads fontset
	for (i = 0; i < 80; i++)
		memory[i] = chip8_fontset[i];
	// loads ROM on memory
	for (i = 0; i < MAX_ROMSIZE; i++)
		memory[512 + i] = rom[i];

	opcode = memory[PC] << 8 | memory[PC + 1];
}

void print_variables() {
	printf("Opcode: %02X%02X\nDelay Timer: %02X (%d)\nSound Timer: %02X (%d)\n", (opcode & 0xFF00) >> 8, opcode & 0x00FF, delay_timer, delay_timer, sound_timer, sound_timer);
	printf("sp: %d\n", sp);
	for (i = 0; i < 12; i++)
		printf("stack[%01X]: %02X\n", i, stack[i]);
}
void print_registers() {
	printf("PC: 0x%04X (%d)\n I: 0x%04X (%d)\n", PC, PC, I, I);
	for (i = 0; i < 16; i++) {
		printf("V%X: 0x%02X (%d)\n", i, V[i], V[i]);
	}
}

void update_framebuffer() {
	for (i = 0; i < 256; i++) {
		framebuffer[8*i+0] = (memory[VRAM + i] & 0x80) >> 7;
		framebuffer[8*i+1] = (memory[VRAM + i] & 0x40) >> 6;
		framebuffer[8*i+2] = (memory[VRAM + i] & 0x20) >> 5;
		framebuffer[8*i+3] = (memory[VRAM + i] & 0x10) >> 4;
		framebuffer[8*i+4] = (memory[VRAM + i] & 0x08) >> 3;
		framebuffer[8*i+5] = (memory[VRAM + i] & 0x04) >> 2;
		framebuffer[8*i+6] = (memory[VRAM + i] & 0x02) >> 1;
		framebuffer[8*i+7] = (memory[VRAM + i] & 0x01) >> 0;
	}
}

void print_framebuffer() {
	update_framebuffer();
	for (i = 0; i < 32; i++) {
		for (j = 0; j < 64; j++) {
			printf("%01X ", framebuffer[64*i+j]);
		}
		printf("\n");
	}
}

void print_memory() {
	printf("               00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
	printf("               -----------------------------------------------\n");
	for (i = 0; i < 256; i++) {
		if (i == 512/16)
			printf("              ---- Starting game memory ----\n");
		if (i == 0x0EA0/16)
			printf("     ---- Starting call stack & internal use memory ----\n");
		if (i == 0x0F00/16)
			printf("              ---- Starting VRAM memory ----\n");
		printf("0x%02X0 (%04d): ", i, 16*i);
		for (j = 0; j < 16; j++) {
			printf("%02X ", memory[16*i+j]);
		}
		printf("\n");
	}
}

void print_opcodes() {
	for (i = 0; i < romsize; i+=2) {
		printf("%03d: ", i/2+1);
		printf("%02X",   rom[i]);
		printf("%02X  opcode: ", rom[i+1]);
		opcode = rom[i] << 8 | rom[i+1];
		print_opcode();
		if (display_description) {
			printf("  ");
			print_opcode_description();
		}
		printf("\n");
	}
}

void chip8_cycle() {
	printf("--- Executing --\n");
	printf("opcode: %02X %02X\n", (opcode & 0xFF00) >> 8, opcode & 0x00FF);

	exec_opcode();

	if (delay_timer > 0)
		delay_timer--;
	if (sound_timer > 0) {
		// emit sound
		sound_timer--;
	}
	printf("----------------\n");
}

int main() {
	int loop = 1;
	int input;

	if (load_rom())
		printf("ROM loaded\n");
	else {
		printf("Unable to load ROM\nExiting");
		goto exit;
	}
	chip8_initialize();

	while (loop) {
		printf("Options:\n\t1: Print registers\n\t2: Print timers/variables\n\t3: Print framebuffer\n\t");
		printf("4: Print rom\n\t5: Print memory\n\t6: Print opcodes\n\t7: Enable/disable opcodes description\n\t");
			printf("9: Next step\n\t0: Exit\n");
		if (display_description)
			printf("\t   Opcodes description enabled\n");
		else
			printf("\t   Opcodes description disabled\n");
		printf("Choose what should be printed: ");
		scanf("%d", &input);
		printf("\n");
		if (input == 1) {
			print_registers();
			printf("\n");
		}
		if (input == 2) {
			print_variables();
			printf("\n");
		}
		if (input == 3) {
			print_framebuffer();
			printf("\n");
		}
		if (input == 4) {
			print_rom();
			printf("\n");
		}
		if (input == 5) {
			print_memory();
			printf("\n");
		}
		if (input == 6) {
			print_opcodes();
			printf("\n");
		}
		if (input == 7) {
			display_description ^= 1;
			printf("Toggled the display of the opcodes' description\n");
		}
		if (input == 9) {
			chip8_cycle();
		}
		if (input == 0) {
			loop = 0;
		}
	}
	exit:
	printf("Closing");
	return 0;
}
