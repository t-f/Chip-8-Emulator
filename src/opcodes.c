#include <stdio.h>

extern unsigned short opcode;
/*
void exec_opcode() {
	switch(opcode & 0xF000) {

	}

}*/

void print_opcode() {
	switch(opcode & 0xF000) {
	case 0x0000:
		if (opcode == 0x00E0) {
			printf("00E0");	break;
		}
		else if (opcode == 0x00EE) {
			printf("00EE");	break;
		}
		else {
		printf("0NNN");	break;
		}
	case 0x1000:
		printf("1NNN");	break;
	case 0x2000:
		printf("2NNN");	break;
	case 0x3000:
		printf("3XNN");	break;
	case 0x4000:
		printf("4XNN");	break;
	case 0x5000:
		printf("5XY0");	break;
	case 0x6000:
		printf("6XNN");	break;
	case 0x7000:
		printf("7XNN");	break;
	case 0x8000:
		if ((opcode & 0xF00F) == 0x8000) {
			printf("8XY0");	break;
		}
		else if ((opcode & 0xF00F) == 0x8001) {
			printf("8XY1");	break;
		}
		else if ((opcode & 0xF00F) == 0x8002) {
			printf("8XY2");	break;
		}
		else if ((opcode & 0xF00F) == 0x8003) {
			printf("8XY3");	break;
		}
		else if ((opcode & 0xF00F) == 0x8004) {
			printf("8XY4");	break;
		}
		else if ((opcode & 0xF00F) == 0x8005) {
			printf("8XY5");	break;
		}
		else if ((opcode & 0xF00F) == 0x8006) {
			printf("8XY6");	break;
		}
		else if ((opcode & 0xF00F) == 0x8007) {
			printf("8XY7");	break;
		}
		else if ((opcode & 0xF00F) == 0x800E) {
			printf("8XYE");	break;
		}
	case 0x9000:
		printf("9XY0");	break;
	case 0xA000:
		printf("ANNN");	break;
	case 0xB000:
		printf("BNNN");	break;
	case 0xC000:
		printf("CXNN");	break;
	case 0xD000:
		printf("DXYN");	break;
	case 0xE000:
		if ((opcode & 0xF0FF) == 0xE09E) {
			printf("EX9E");	break;
		}
		else if ((opcode & 0xF0FF) == 0xE0A1) {
			printf("EXA1");	break;
		}
	case 0xF000:
		if ((opcode & 0xF0FF) == 0xF007) {
			printf("FX07");	break;
		}
		else if((opcode & 0xF0FF) == 0xF00A) {
			printf("FX0A");	break;
		}
		else if((opcode & 0xF0FF) == 0xF015) {
			printf("FX15");	break;
		}
		else if((opcode & 0xF0FF) == 0xF018) {
			printf("FX18");	break;
		}
		else if((opcode & 0xF0FF) == 0xF01E) {
			printf("FX1E");	break;
		}
		else if((opcode & 0xF0FF) == 0xF029) {
			printf("FX29");	break;
		}
		else if((opcode & 0xF0FF) == 0xF033) {
			printf("FX33");	break;
		}
		else if((opcode & 0xF0FF) == 0xF055) {
			printf("FX55");	break;
		}
		else if((opcode & 0xF0FF) == 0xF065) {
			printf("FX65");	break;
		}
	}
}