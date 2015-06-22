#include <stdio.h> 	// printf(), scanf(), FILE, fopen(), fread()
#include <stdlib.h> // rand(), srand()
#include <stdarg.h> // va_list, va_start(), vsprintf(), va_end()
#include <string.h> // strlen()
#include <time.h> // time()
#include <math.h>
#include <SDL2/SDL.h>
#undef main

#define PI M_PI
#define CW 10 	//char width

#define MAX_ROMSIZE 0xCA0
#define VRAM 0xF00

#define MAIN_BG_COLOR_R 0xBA 	// gamma corrected 0x7F7F7F
#define MAIN_BG_COLOR_G 0xBA
#define MAIN_BG_COLOR_B 0xBA

#define GAME_FG_COLOR_RGB 0xFFFFFF

void print_opcode();
void print_opcode_description();
void exec_opcode();
void dtext(int x, int y, const char * format, ...);

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

int i, j, k, l;
int display_description = 1;

SDL_Window* 	window = NULL;
SDL_Renderer* 	renderer = NULL;
SDL_AudioSpec   as;
SDL_Event 		e;

SDL_Rect 		desktop_rect;
SDL_Rect 		window_rect;
SDL_Rect 		font_src_rect  = {0,0,CW,CW}; 	// character area
SDL_Rect 		font_dest_rect = {0,0,CW,CW}; 	// destination area (screen)
SDL_Rect 		screen_src_rect  = {0, 0, 64, 32};
SDL_Rect 		screen_dest_rect = {0, 0, 64, 32};
SDL_Texture* 	font_texture = NULL;
SDL_Texture* 	video_texture = NULL;
SDL_Surface* 	font_surface = NULL;
SDL_Surface* 	video_surface = NULL;
SDL_Surface* 	tmp_surface = NULL;
char* 			fontname = "./res/10x10C.bmp";

unsigned short opcode;
unsigned char memory[4096];

unsigned char V[16];
unsigned short I;
unsigned short PC; // Program/instruction pointer

unsigned char delay_timer;
unsigned char sound_timer;

Uint32 framebuffer[64*32]; // 2048 pixels

unsigned short stack[16];
unsigned short sp; // stack pointer

unsigned char key[16]; // keyboard current state
unsigned char rom[MAX_ROMSIZE]; // ROM will be loaded here
unsigned int romsize;

static Uint64 old_1, old_2, old_3;
unsigned int instructions_per_second = 1000;

SDL_Point sprite_line[8];

int load_rom(const char* romname) {
	FILE* fp = fopen(romname, "rb");
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
	//printf("Opcode: %02X%02X\nDelay Timer: %02X (%d)\nSound Timer: %02X (%d)\n", (opcode & 0xFF00) >> 8, opcode & 0x00FF, delay_timer, delay_timer, sound_timer, sound_timer);
	dtext(40, 1, "Opcode: %02X%02X\nDelay Timer: %02X (%d)\nSound Timer: %02X (%d)\n", (opcode & 0xFF00) >> 8, opcode & 0x00FF, delay_timer, delay_timer, sound_timer, sound_timer);
	//printf("sp: %d\n", sp);
	dtext(40, 5, "sp: %d\n", sp);
	for (i = 0; i < 12; i++)
		//printf("stack[%01X]: %02X\n", i, stack[i]);
		dtext(40, 7+i, "stack[%01X]: %02X\n", i, stack[i]);
}
void print_registers() {
	//printf("PC: 0x%04X (%d)\n I: 0x%04X (%d)\n", PC, PC, I, I);
	dtext(1, 23, "PC: 0x%04X (%d)\n I: 0x%04X (%d)\n", PC, PC, I, I);
	for (i = 0; i < 16; i++) {
		//printf("V%X: 0x%02X (%d)\n", i, V[i], V[i]);
		dtext(1, 26+i, "V%X: 0x%02X (%d)\n", i, V[i], V[i]);
	}
}

void update_framebuffer() {
	for (i = 0; i < 256; i++) {
		framebuffer[8*i+0] = ((memory[VRAM + i] & 0x80) >> 7)*GAME_FG_COLOR_RGB;
		framebuffer[8*i+1] = ((memory[VRAM + i] & 0x40) >> 6)*GAME_FG_COLOR_RGB;
		framebuffer[8*i+2] = ((memory[VRAM + i] & 0x20) >> 5)*GAME_FG_COLOR_RGB;
		framebuffer[8*i+3] = ((memory[VRAM + i] & 0x10) >> 4)*GAME_FG_COLOR_RGB;
		framebuffer[8*i+4] = ((memory[VRAM + i] & 0x08) >> 3)*GAME_FG_COLOR_RGB;
		framebuffer[8*i+5] = ((memory[VRAM + i] & 0x04) >> 2)*GAME_FG_COLOR_RGB;
		framebuffer[8*i+6] = ((memory[VRAM + i] & 0x02) >> 1)*GAME_FG_COLOR_RGB;
		framebuffer[8*i+7] = ((memory[VRAM + i] & 0x01) >> 0)*GAME_FG_COLOR_RGB;
	}
}
/*
void print_framebuffer() {
	update_framebuffer();
	for (i = 0; i < 32; i++) {
		for (j = 0; j < 64; j++) {
			printf("%01X ", framebuffer[64*i+j]);
		}
		printf("\n");
	}
}
*/
void update_screen(int scale) {
	screen_dest_rect.w = 64*scale;
	screen_dest_rect.h = 32*scale;
	update_framebuffer();
	video_surface = SDL_CreateRGBSurfaceFrom(framebuffer, 64, 32, 32, 64*4, 0, 0, 0, 0);
	video_texture = SDL_CreateTextureFromSurface(renderer, video_surface);
	SDL_RenderCopy(renderer, video_texture, &screen_src_rect, &screen_dest_rect);
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
	opcode = memory[PC] << 8 | memory[PC + 1];
}

void chip8_cycle() {
	printf("--- Executing %02X %02X\n", (opcode & 0xFF00) >> 8, opcode & 0x00FF);
	printf("\t\t\t\t%d instructions per second\n", instructions_per_second);
	char title_string[100];
	sprintf(title_string, "Chip-8 Emulator %d instructions/second", instructions_per_second);
	SDL_SetWindowTitle(window, title_string);

	Uint64 new = SDL_GetPerformanceCounter();

	if (delay_timer > 0) {
		if ((new - old_1)*1.0/SDL_GetPerformanceFrequency() >= 1.0/60) {
			delay_timer --;
			old_1 = SDL_GetPerformanceCounter();
		}
	}
	if (sound_timer > 0) {
		SDL_PauseAudio(0);
		if ((new - old_2)*1.0/SDL_GetPerformanceFrequency() >= 1.0/60) {
			sound_timer --;
			old_2 = SDL_GetPerformanceCounter();
		}
	}
	else {
		SDL_PauseAudio(1);
	}

	if ((new - old_3)*1.0/SDL_GetPerformanceFrequency() >= 1.0/instructions_per_second) {
		exec_opcode();
		for (i = 0; i < 16; i++)
			key[i] = 0;
		old_3 = SDL_GetPerformanceCounter();
	}
}

void fill_audio(void *data, Uint8 *stream, int len) {
	short *buff;
	int i;

	buff = (short*)stream;
	len /= 2; //shorts

	//sine wave
	static unsigned int t = 0;
	int volume = 15000;
	int freq1 = 440*pow(2,(-19)/12.0);
	int freq2 = 440*pow(2,(-7)/12.0);
	for (i = 0; i < len; i++) {
		buff[i] = volume/2.0*sin(t*2*PI*freq1/44100);
		buff[i] += volume/2.0*sin(t*2*PI*freq2/44100);
		t++;
	}
}

void dtext(int x, int y, const char * format, ...) {
	int i;
	char c[256]={0};

	va_list args;
	va_start(args, format);
	vsprintf(c, format, args);
	va_end(args);

	font_dest_rect.x = x*CW;
	font_dest_rect.y = y*CW;

	for(i = 0; i < strlen(c); i++) {
		font_src_rect.x = CW*(int)(c[i]%16);
		font_src_rect.y = CW*(int)(c[i]/16);
		if (c[i] == '\n') {
			font_dest_rect.x = x*CW;
			font_dest_rect.y += CW;
		}
		else {
			SDL_RenderCopy(renderer, font_texture, &font_src_rect, &font_dest_rect);
			font_dest_rect.x += CW;
		}
	}
}

int main(int argc, const char *argv[]) {
	int quit = 0;
	int run_game = 1;
	int screen_scale = 10;

	if (argc == 2) {
		if (load_rom(argv[1]))
			printf("\nROM loaded\n\n");
		else {
			printf("Unable to load ROM\nExiting\n\n");
			goto exit;
		}
	}
	else {
		printf("add the ROM name as the only parameter\n\n");
		goto exit;
	}

	srand (time(NULL));
	SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
	window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 320, 0);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(renderer, MAIN_BG_COLOR_R, MAIN_BG_COLOR_G, MAIN_BG_COLOR_R, 255);
	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	//SDL_RenderSetLogicalSize(renderer, 64, 32);
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	old_1 = SDL_GetPerformanceCounter();
	old_2 = SDL_GetPerformanceCounter();
	old_3 = SDL_GetPerformanceCounter();

	as.freq = 44100;
	as.format = AUDIO_S16SYS;
	as.channels = 1;
	as.samples = 1024;
	as.callback = fill_audio;

	SDL_OpenAudio(&as,NULL);

	if (font_surface == NULL) {
		tmp_surface = SDL_LoadBMP(fontname);
		SDL_SetColorKey(tmp_surface, SDL_TRUE, SDL_MapRGB(tmp_surface->format, 0xFF, 0, 0xFF));
		font_texture = SDL_CreateTextureFromSurface(renderer, tmp_surface);
		SDL_FreeSurface(tmp_surface);
	}

	chip8_initialize();
	printf(" keys     mapped to\n");
	printf("1 2 3 4    1 2 3 C\nQ W E R    4 5 6 D\nA S D F -> 7 8 9 E\nZ X C V    A 0 B F\n\n");

	while (!quit) {
		//SDL_SetRenderDrawColor(renderer, MAIN_BG_COLOR_R, MAIN_BG_COLOR_G, MAIN_BG_COLOR_B, 255);
		SDL_RenderClear(renderer);
		//SDL_SetRenderDrawColor(renderer, FG_COLOR_R, FG_COLOR_G, FG_COLOR_B, 255);

		while(SDL_PollEvent(&e) != 0) {
			if(e.type == SDL_QUIT)
				quit = 1;
			if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_ESCAPE)
					quit = 1;
				if (e.key.keysym.sym == SDLK_7) {
					printf("Disabled\n");
				}
				if (e.key.keysym.sym == SDLK_8) {
					printf("Disabled\n");
				}
				if (e.key.keysym.sym == SDLK_9) {
					//print_framebuffer();
					printf("Disabled\n");
				}
				if (e.key.keysym.sym == SDLK_0) {
					print_rom();
					printf("\n");
				}
				if (e.key.keysym.sym == SDLK_u) {
					print_memory();
					printf("\n");
				}
				if (e.key.keysym.sym == SDLK_i) {
					print_opcodes();
					printf("\n");
				}
				if (e.key.keysym.sym == SDLK_o) {
					display_description ^= 1;
					printf("Toggled the display of the opcodes' description\n");
				}
				if (e.key.keysym.sym == SDLK_p) {
					chip8_cycle();
				}
				if (e.key.keysym.sym == SDLK_j) {
					SDL_GetWindowPosition(window, &window_rect.x, &window_rect.y);
					SDL_GetWindowSize(window, &window_rect.w, &window_rect.h);
					SDL_GetDisplayBounds(0, &desktop_rect);
					if ((window_rect.x) < 0)
						window_rect.x = 0;
					else if ((window_rect.x+window_rect.w) > desktop_rect.w)
						window_rect.x = desktop_rect.w - window_rect.w;
					if ((window_rect.y) < 0)
						window_rect.y = 0;
					else if ((window_rect.y+window_rect.h) > desktop_rect.h)
						window_rect.y = desktop_rect.h - window_rect.h;

					run_game ^= 1;
					if (run_game) {
						if ((window_rect.x+640/2) > (desktop_rect.w/2))
							window_rect.x += (800-640);
						if ((window_rect.y+320/2) > (desktop_rect.h/2))
							window_rect.y += (600-320);
						SDL_SetWindowPosition(window, window_rect.x, window_rect.y);
						SDL_SetWindowSize(window, 640, 320);
						screen_scale = 10;
					}
					else {
						if ((window_rect.x+640/2) > (desktop_rect.w/2))
							window_rect.x -= (800-640);
						if ((window_rect.y+320/2) > (desktop_rect.h/2))
							window_rect.y -= (600-320);
						SDL_SetWindowPosition(window, window_rect.x, window_rect.y);
						SDL_SetWindowSize(window, 800, 600);
						screen_scale = 5;

					}
				}
				if (e.key.keysym.sym == SDLK_UP) {
					instructions_per_second += 100;
				}
				if (e.key.keysym.sym == SDLK_DOWN) {
					instructions_per_second -= 100;
				}
				if (e.key.keysym.sym == SDLK_LEFT) {
					instructions_per_second -= 10;
				}
				if (e.key.keysym.sym == SDLK_RIGHT) {
					instructions_per_second += 10;
				}
				if (instructions_per_second <= 0)
					instructions_per_second = 100;
				printf("\n--- Next instruction ---\n");
				printf("PC: 0x%04X | opcode: %02X %02X\n", PC, (opcode & 0xFF00) >> 8, opcode & 0x00FF);
			}
			if(e.type == SDL_WINDOWEVENT) {
				if (e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
					printf("Options:\n\t7: Print registers\n\t8: Print timers/variables\n\t9: Print framebuffer\n\t");
					printf("0: Print rom\n\tU: Print memory\n\tI: Print opcodes\n\tO: Enable/disable opcodes description\n\t");
					printf("P: Next step\n\tJ: Toggle play game/debug\n\tArrows: UP/DOWN +/- 100 ins/s, RIGHT/LEFT +/- 10 ins/s\n");
					printf("\tEsc: Exit\n");
					if (display_description)
						printf("\t   Opcodes description enabled\n");
					else
						printf("\t   Opcodes description disabled\n");
					printf("\n");
					printf("\n--- Next instruction ---\n");
					printf("PC: 0x%04X | opcode: %02X %02X\n", PC, (opcode & 0xFF00) >> 8, opcode & 0x00FF);
				}
				if (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
						printf("Lost focus. Keyboard input is read from the graphics window only\n\n");
				}
			}
		}
		if (keystate[SDL_SCANCODE_1])
			key[1] = 1;
		if (keystate[SDL_SCANCODE_2])
			key[2] = 1;
		if (keystate[SDL_SCANCODE_3])
			key[3] = 1;
		if (keystate[SDL_SCANCODE_Q])
			key[4] = 1;
		if (keystate[SDL_SCANCODE_W])
			key[5] = 1;
		if (keystate[SDL_SCANCODE_E])
			key[6] = 1;
		if (keystate[SDL_SCANCODE_A])
			key[7] = 1;
		if (keystate[SDL_SCANCODE_S])
			key[8] = 1;
		if (keystate[SDL_SCANCODE_D])
			key[9] = 1;
		if (keystate[SDL_SCANCODE_X])
			key[0] = 1;
		if (keystate[SDL_SCANCODE_Z])
			key[0xA] = 1;
		if (keystate[SDL_SCANCODE_C])
			key[0xB] = 1;
		if (keystate[SDL_SCANCODE_4])
			key[0xC] = 1;
		if (keystate[SDL_SCANCODE_R])
			key[0xD] = 1;
		if (keystate[SDL_SCANCODE_F])
			key[0xE] = 1;
		if (keystate[SDL_SCANCODE_V])
			key[0xF] = 1;

		if (run_game) {
			chip8_cycle();
		}
		if (!run_game) {
			SDL_GetWindowPosition(window, &window_rect.x, &window_rect.y);
			SDL_GetWindowSize(window, &window_rect.w, &window_rect.h);
			SDL_GetDisplayBounds(0, &desktop_rect);
			dtext(1, 20, "x:%d, y:%d, w:%d, h:%d", desktop_rect.x, desktop_rect.y, desktop_rect.w, desktop_rect.h);
			dtext(1, 21, "x:%d, y:%d, w:%d, h:%d", window_rect.x, window_rect.y, window_rect.w, window_rect.h);
			print_registers();
			print_variables();
		}
		update_screen(screen_scale);
		SDL_RenderPresent(renderer);
	}
	exit:
	printf("Closing");
	return 0;
}
