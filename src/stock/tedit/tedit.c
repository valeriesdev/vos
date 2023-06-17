#include <stdint.h>
#include <stddef.h>
#include "stock/tedit/tedit.h"
#include "drivers/keyboard.h"
#include "drivers/screen.h"
#include "cpu/ports.h"
#include "cpu/isr.h"
#include "libc/string.h"
#include "libc/function.h"
#include "kernel/kernel.h"
#include "libc/mem.h"

char *top_row = "[VOS TEdit v0.0] ctrl+s: save | ctrl+q: exit | File:";
char *file_creation_row = "Enter filename:";
char *file_saved_at = "file saved at";
char* filename;
char* file;

void draw_ui() {
	kprint_at(top_row, 0, 0);
	kprint_at(filename, 52, 0);
}

void get_filename() {
	kprint_at(file_creation_row, 0, 1);
	if(filename != 0x0) free(filename);
	filename = malloc(sizeof(char)*256);

	uint8_t *keycodes = malloc(sizeof(uint8_t)*3);
    keycodes[0] = 0x1C; keycodes[1] = 0x0; keycodes[2] = 0x0;
    void (**gcallback_functions)() = malloc(sizeof(void*)*10);
    *gcallback_functions = filename_recieved;
    struct keyboard_initializer* keyboardi = create_initializer(filename,
                                                                1,
                                                                keycodes,
                                                                gcallback_functions,
                                                                0x0);
    init_keyboard(keyboardi);
}

void filename_recieved() {
	int i = 0;
	for(i = 0; i < 256; i++) {
		if(key_buffer[i] == '\0') break;
	}
	char* new_string = malloc(i*sizeof(char));
	memory_copy((uint8_t *)key_buffer, (uint8_t *)new_string, i);
	key_buffer[0] = '\0';
	
	filename = new_string;
	kprintn(filename);
	//rehook_keyboard(file_writing_hook);

	clear_screen();
	draw_ui();
}

void file_save() {
	int i = 0;
	for(i = 0; i < 1024; i++) {
		if(file[i] == '\0') break;
	}
	char* ram_saved_file = malloc(sizeof(char) * i);
	memory_copy((uint8_t*)file, (uint8_t*)ram_saved_file, i);
	kprint_at(file_saved_at, 0, 0);
	kprint_at(hex_to_ascii((int)ram_saved_file), 13, 0);
}

void launch_tedit(char *args) {
	file = malloc(sizeof(char) * 1024);
	clear_screen();
	draw_ui();

	get_filename();
}

void resolve_program_close() {
	uint8_t *keycodes = malloc(sizeof(uint8_t)*3);
    keycodes[0] = 0x1C; keycodes[1] = 0x0; keycodes[2] = 0x0;
    void (**gcallback_functions)() = malloc(sizeof(void*)*10);
    *gcallback_functions = user_input;
    struct keyboard_initializer* keyboardi = create_initializer(malloc(sizeof(char)*256),
                                                                1,
                                                                keycodes,
                                                                gcallback_functions,
                                                                0x0);
    init_keyboard(keyboardi);
}