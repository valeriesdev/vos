#include <stdint.h>
#include "drivers/keyboard.h"
#include "drivers/screen.h"
#include "kernel/kernel.h"
#include "libc/mem.h"
#include "libc/string.h"
#include "filesystem/filesystem.h"
#include "stock/tedit/tedit.h"
#include "stock/program_interface/popup.h"

#define TRUE 1
#define FALSE 0

// Functions
static void initialize_keyboard();
static void exit_program();
static void initialize();
static void save_program();

// Variables that will need to be malloc'd and free'd
char* keybuffer;

// Variables
uint8_t new_file;
char *file_name;
uint8_t exit;

static void initialize_keyboard() {
	keybuffer = malloc(sizeof(char)*256);
	void (*gcallback_functions[])() = {exit_program, save_program, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
	uint8_t keycodes[] = {0x1D, 0x10, 0x0, 0x1D, 0x1F, 0x0};
    
    struct keyboard_initializer* keyboardi = create_initializer(2, keycodes, gcallback_functions, 0x0, keybuffer);
    init_keyboard(keyboardi);
}

static void exit_program() {
	// Free all memory related to program
	keybuffer = free(keybuffer);

	// Prepare return to CLI
	clear_screen();
	kprint("\n> ");

	// Init CLI Keyboard
	kernel_init_keyboard();
	exit = 0;
}

static void save_program() {
	append(keybuffer,'\0');
	if(new_file) {
		write_file(file_name, keybuffer, strlen(keybuffer));
	} else {
		overwrite_file(file_name, keybuffer, strlen(keybuffer));
	}

	struct file* files = get_files()+1;
	while(files->magic == 0xFFFFFFFF) {
		kprintn(files->name);
		files++;
	}
	backspace(keybuffer);
}

static void initialize() {
	clear_screen();

	struct popup_str_struct* z = malloc(sizeof(struct popup_str_struct));
	*z = (struct popup_str_struct) {5,20,5,15,6,"Enter file name: ", 12, NULL};
	file_name = create_popup(1, z);
	z = free(z);

	initialize_keyboard();
	
	struct file* files = get_files()+1;
	while(files->magic == 0xFFFFFFFF) {
		if(strcmp(files->name,file_name) == 0) break;
		files++;
	}
	if(files->magic != 0xFFFFFFFF) {
		// procedure for new file
		new_file = TRUE;
	} else {
		// procedure for editing file
		new_file = FALSE;
		// Load file from disk
		// Copy file into keybuffer
		// Print keybuffer
		void* old_file = read_file(file_name);
		memory_copy((uint8_t*)old_file,(uint8_t*)keybuffer,strlen((char*)old_file));
		kprint_at(keybuffer,0,1);
		free(old_file);
	}

	// Static screen text
	char* header_00 = "0x00000 | VOS TEdit 0.0 | File: ";
	char* header_01 = " | New file?: ";
	char* footer_00 = "Press ctrl+q to exit, ctrl+s to save.";

	kprint_at(header_00,0,0);
	kprint(file_name);
	kprint(header_01);
	if(new_file) kprint("YES");
	else kprint("NO");
	kprint_at_preserve(footer_00,0,24);
	kprint("\n");

	exit = 1;
	while(exit);
}

void tedit() {
	initialize();
}