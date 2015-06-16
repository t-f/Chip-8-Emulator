#include <stdio.h> 	// printf(), scanf(), FILE, fopen(), fread()

#define MAX_ROMSIZE 0xCA0

void print_opcode();
void print_opcode_description();

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
	opcode = 0;
	I = 0;
	sp = 0;

	// loads fontset
	for (i = 0; i < 80; i++)
		memory[i] = chip8_fontset[i];
	// loads ROM on memory
	for (i = 0; i < MAX_ROMSIZE; i++)
		memory[512 + i] = rom[i];
}

void print_variables() {
	printf("Opcode: %d\nDelay Timer: %d\nSound Timer: %d\n", opcode, delay_timer, sound_timer);
}
void print_registers() {
	printf("PC: %d\n I: %d\n", PC, I);
	for (i = 0; i < 16; i++) {
		printf("V%X: %d\n", i, V[i]);
	}
}

void print_framebuffer() {
	for (i = 0; i < 32; i++) {
		for (j = 0; j < 64; j++) {
			printf("%d", framebuffer[64*i+j]);
		}
		printf("\n");
	}
}

void print_memory() {
	for (i = 0; i < 128; i++) {
		for (j = 0; j < 32; j++) {
			printf("%02X", memory[32*i+j]);
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

int main() {
	int loop = 1;
	int input;
	while (loop) {
		if (load_rom()) {
			printf("ROM loaded\n");
		}
		else {
			printf("Unable to load ROM\nExiting");
			break;
		}
		chip8_initialize();
		printf("Options:\n\t1: Print registers\n\t2: Print timers/variables\n\t3: Print framebuffer\n\t");
		printf("4: Print rom\n\t5: Print memory\n\t6: Print opcodes\n\t7: Enable/disable opcodes description\n\t0: Exit\n");
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
		if (input == 0) {
			loop = 0;
		}
	}
	printf("Closing");
	return 0;
}
