#include <stdint.h>
#include <stddef.h>
#include "tedit.h"

const char* top_row = "[VOS TEdit v0.0] ctrl+s: save | ctrl+q: exit | File:";
const char* file_creation_row = "Enter filename:";
char* filename;

void get_filename() {
	kprint_at(file_creation_row, 0, 1);
	rehook_keyboard(get_string_hook);
}

void filename_recieved() {
	int i = 0;
	for(i = 0; i < 256; i++) {
		if(key_buffer[i] == '\0') break;
		//kprint(key_buffer[i]);
	}
	char* new_string = malloc(i*sizeof(char));
	memory_copy(key_buffer, new_string, i);
	key_buffer[0] = '\0';
	
	filename = new_string;
	kprintn(filename);
	rehook_keyboard(file_writing_hook);

	//clear_screen();
	//draw_ui();
}

void launch_tedit(char *args) {
	clear_screen();
	draw_ui();
	init_special_keys();
	get_filename();
}

void draw_ui() {
	kprint_at(top_row, 0, 0);
	kprint_at(filename, 52, 0);
}

void resolve_program_close() {
	init_keyboard();
}