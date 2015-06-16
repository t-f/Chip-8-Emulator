#include <stdio.h>

int i, j;

unsigned short opcode;
unsigned char memory[4096];

unsigned char V[16];
unsigned short I;
unsigned short PC; // Program/instruction pointer

unsigned char delay_timer;
unsigned char sound_timer;

unsigned char framebuffer[64*32]; // 2048 pixels

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
			printf("%d", framebuffer[32*j+i]);
		}
		printf("\n");
	}
}

int main() {
	int loop = 1;
	int input;
	while (loop) {
		printf("Options:\n\t1: Print registers\n\t2: Print timers/variables\n\t3:Print framebuffer\n\t0: Exit\n");
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
		if (input == 0) {
			loop = 0;
		}
	}
	printf("Closing");
	return 0;
}
