#include <stdint.h>

#include "drivers/keyboard.h"
#include "drivers/screen.h"
#include "kernel/kernel.h"
#include "libc/mem.h"
#include "stock/program_interface/program_interface.h"
#include "stock/program_interface/popup.h"

// Functions
void initialize_keyboard();
void exit_program();
void initialize();

// Variables that will need to be malloc'd and free'd
uint8_t *keycodes;
void (**gcallback_functions)();
char* keybuffer;

void initialize_keyboard() {
	keybuffer = malloc(sizeof(char)*256);
	// Initialize space to store key callbacks
	keycodes = malloc(sizeof(uint8_t)*3);
	gcallback_functions = malloc(sizeof(void*)*10);

    // Set ctrl+q callback to exit the program
    keycodes[0] = 0x1D; keycodes[1] = 0x10; keycodes[2] = 0x0;
    *gcallback_functions = exit_program;
    
    struct keyboard_initializer* keyboardi = create_initializer(keybuffer, 1, keycodes, gcallback_functions, 0x0);
    init_keyboard(keyboardi);
}

void exit_program() {
	// Free all memory related to program
	free(keybuffer);
	free(gcallback_functions);
	free(keybuffer);

	// Prepare return to CLI
	clear_screen();
	kprint("> ");

	// Init CLI Keyboard
	kernel_init_keyboard();
}

void initialize() {
	initialize_keyboard();

	clear_screen();

	struct popup_msg_struct* s = malloc(sizeof(struct popup_msg_struct));
	*s = (struct popup_msg_struct) {5,20,5,15,6,"Popup!"};
	create_popup(0, s);
	free(s);

	struct popup_str_struct* z = malloc(sizeof(struct popup_str_struct));
	*z = (struct popup_str_struct) {5,20,5,15,6,"Popup!", 12, "null"};
	create_popup(1, z);
	free(s);
}