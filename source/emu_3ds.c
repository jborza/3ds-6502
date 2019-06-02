#include "emu_3ds.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "disassembler.h"
#include "cpu.h"

State6502 state;
int last_key;

int paused = 1;
int show_diagnostics = 1;
int show_memory = 0;

int frameskip = 128;

void initialize_state(){
	clear_state(&state);
	state.memory = malloc(MEMORY_SIZE);
	memset(state.memory, 0, MEMORY_SIZE);
}

void load_bin_from_memory(){
    //TODO let user decide which binary to load
    //tiny program for now
    char bin[] = {0xa2, 0x01, 0x8e, 0x00, 0x02, 0xe8, 0x8e, 0x01, 0x02 };
   	memcpy(state.memory + PRG_START, bin, 9);
}

byte * read_bin(const char *filename, int* bin_file_size) {
	FILE* file = fopen(filename, "rb");
	if (!file) {
		exit(1);
	}
	fseek(file, 0, SEEK_END);
	*bin_file_size = ftell(file);

	rewind(file);

	byte* buffer = malloc(*bin_file_size);
	size_t read = fread(buffer, sizeof(byte), *bin_file_size, file);
	fclose(file);
	return buffer;
}

void process_direction_keys(u32 kDown){
    if (kDown & KEY_LEFT)
        last_key = 'a';
    else if (kDown & KEY_DOWN)
        last_key = 's';
    else if (kDown & KEY_RIGHT)
        last_key = 'd';
    else if (kDown & KEY_UP)
        last_key = 'w';
}

void load_bin(){
    int bin_file_size = 0;
    // byte *bin = read_bin("bins/gameoflife.bin", &bin_file_size);
    byte *bin = read_bin("bins/snake.bin", &bin_file_size);
   	memcpy(state.memory + PRG_START, bin, bin_file_size);
}

void reset_pc(){
    state.pc = PRG_START;
}

void reset_emulation(){
    initialize_state();
    load_bin();
    reset_pc();
}

void print_state(){
    if(!show_diagnostics)
        return;
    printf("\x1b[1;1HA=$%02X X=$%02X Y=$%02X", state.a, state.x, state.y);
	printf("\x1b[2;1HSP=$%02X PC=$%04X", state.sp, state.pc);
	printf("\x1b[3;1HNV-BDIZC");
	
	printf("\x1b[4;1H%d%d%d%d%d%d%d%d", 
    state.flags.n, state.flags.v, state.flags.pad, state.flags.b, state.flags.d, state.flags.i, state.flags.z, state.flags.c);

    char* buffer = disassemble_6502_to_string(state.memory, state.pc);
    printf("\x1b[6;1H%s", buffer);
}

void emu_tick(){
    state.memory[0xFF] = last_key & 0xFF;
    //generate random
	state.memory[0xfe] = rand() & 0xFF;
    if(state.flags.b != 1)
	    emulate_6502_op(&state);
}