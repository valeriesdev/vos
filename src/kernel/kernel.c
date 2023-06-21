#include <stdint.h>
#include "cpu/isr.h"
#include "drivers/screen.h"
#include "drivers/ata.h"
#include "libc/string.h"
#include "libc/mem.h"
#include "kernel/kernel.h"
#include "kernel/commands.h"
#include "stock/tedit/tedit.h"
#include "filesystem/filesystem.h"
#include "drivers/keyboard.h"

struct command_block command_resolver_head;

void kernel_main() {
    initialize_memory();
    load_fat_from_disk();

    isr_install();
    irq_install();

    kprint("Welcome to VOS!\n> ");

    struct command_block temporary_head = {NULLFUNC, "", NULL};
    command_resolver_head = temporary_head;

    register_command(&command_resolver_head, END, "END");
    register_command(&command_resolver_head, PAGE, "PAGE");
    register_command(&command_resolver_head, ECHO, "ECHO");
    register_command(&command_resolver_head, launch_tedit, "tedit"); 

    char* tfile_name = "HELLOWORLDTXT";
    char* exfiledata = "HELLO WORLD\nTHIS IS AN EXAMPLE FILE!!!!\n YAY!!!!\n";
    uint8_t s = 49;
    write_file(tfile_name, exfiledata, s);
    write_file(tfile_name, exfiledata, s);
    write_file(tfile_name, exfiledata, s);
    write_file(tfile_name, exfiledata, s);

    fs_debug();
}


void user_input(char *input) {
    kprint("\n");
    resolve_command(command_resolver_head, str_split(key_buffer, ' ')[0])(key_buffer);
    kprint("> ");

    key_buffer[0] = '\0';
}

void kernel_init_keyboard() {
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