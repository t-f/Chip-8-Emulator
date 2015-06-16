#include <stdio.h>

void print_opcode();
void print_opcode_description();
int return_opcode();

extern unsigned short opcode;
extern unsigned char memory[4096];
extern unsigned char V[16];
extern unsigned short I;
extern unsigned short PC;

enum opcode_enum{
	_0NNN = 1,
	_00E0,
	_00EE,
	_1NNN,
	_2NNN,
	_3XNN,
	_4XNN,
	_5XY0,
	_6XNN,
	_7XNN = 10,
	_8XY0,
	_8XY1,
	_8XY2,
	_8XY3,
	_8XY4,
	_8XY5,
	_8XY6,
	_8XY7,
	_8XYE,
	_9XY0 = 20,
	_ANNN,
	_BNNN,
	_CXNN,
	_DXYN,
	_EX9E,
	_EXA1,
	_FX07,
	_FX0A,
	_FX15,
	_FX18 = 30,
	_FX1E,
	_FX29,
	_FX33,
	_FX55,
	_FX65 = 35
};

const char* opcode_array[36] = {
	"",
	"0NNN",
	"00E0",
	"00EE",
	"1NNN",
	"2NNN",
	"3XNN",
	"4XNN",
	"5XY0",
	"6XNN",
	"7XNN",
	"8XY0",
	"8XY1",
	"8XY2",
	"8XY3",
	"8XY4",
	"8XY5",
	"8XY6",
	"8XY7",
	"8XYE",
	"9XY0",
	"ANNN",
	"BNNN",
	"CXNN",
	"DXYN",
	"EX9E",
	"EXA1",
	"FX07",
	"FX0A",
	"FX15",
	"FX18",
	"FX1E",
	"FX29",
	"FX33",
	"FX55",
	"FX65"
};

const char* opcode_description_array[36] = {
	"",
	"Calls RCA 1802 program at address NNN.",
	"Clears the screen.",
	"Returns from a subroutine.",
	"Jumps to address NNN.",
	"Calls subroutine at NNN.",
	"Skips the next instruction if VX equals NN.",
	"Skips the next instruction if VX doesn't equal NN.",
	"Skips the next instruction if VX equals VY.",
	"Sets VX to NN.",
	"Adds NN to VX.",
	"Sets VX to the value of VY.",
	"Sets VX to VX or VY.",
	"Sets VX to VX and VY.",
	"Sets VX to VX xor VY.",
	"Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.",
	"VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.",
	"Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.",
	"Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.",
	"Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.",
	"Skips the next instruction if VX doesn't equal VY.",
	"Sets I to the address NNN.",
	"Jumps to the address NNN plus V0.",
	"Sets VX to a random number, masked by NN.",
	"Sprites stored in memory at location in index register (I), maximum 8bits wide. Wraps around the screen. If when drawn, clears a pixel, register VF is set to 1 otherwise it is zero. All drawing is XOR drawing",
	"Skips the next instruction if the key stored in VX is pressed.",
	"Skips the next instruction if the key stored in VX isn't pressed.",
	"Sets VX to the value of the delay timer.",
	"A key press is awaited, and then stored in VX.",
	"Sets the delay timer to VX.",
	"Sets the sound timer to VX.",
	"Adds VX to I.",
	"Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.",
	"Stores the Binary-coded decimal representation of VX, with the most significant of three digits at the address in I, the middle digit at I plus 1, and the least significant digit at I plus 2.",
	"Stores V0 to VX in memory starting at address I.",
	"Fills V0 to VX with values from memory starting at address I."
};

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
		printf("V[%01X] = %d\n", X, V[X]);
		printf("to\n");
		V[X] = N;
		printf("V[%01X] = %d\n", X, V[X]);
		PC += 2;
		opcode = memory[PC] << 8 | memory[PC + 1];
		break;
	}
}
