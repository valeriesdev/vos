/**
 * @defgroup   KERNEL kernel
 *
 * @brief      The operating system kernel entry point
 *
 * @author     Valerie Whitmire
 * @date       2023
 */
#include <stdint.h>
#include "cpu/isr.h"
#include "drivers/screen.h"
#include "drivers/ata.h"
#include "libc/string.h"
#include "libc/mem.h"
#include "kernel/kernel.h"
#include "kernel/commands.h"
#include "stock/program_interface/program_interface.h"
#include "filesystem/filesystem.h"
#include "drivers/keyboard.h"

struct command_block *command_resolver_head;
char **lkeybuffer;

void kernel_main() {
    initialize_memory();
    load_fat_from_disk();

    isr_install();
    irq_install();

    kprint("Welcome to VOS!\n> ");

    command_resolver_head = malloc(sizeof(struct command_block));
    command_resolver_head->function = NULLFUNC;
    command_resolver_head->call_string = "";
    command_resolver_head->next = NULL;

    register_command(command_resolver_head, END, "end");
    register_command(command_resolver_head, PAGE, "page");
    register_command(command_resolver_head, ECHO, "echo");
    register_command(command_resolver_head, initialize, "initt"); 
    register_command(command_resolver_head, LS, "ls");
    register_command(command_resolver_head, HELP, "help");

    /*char* tfile_name = "HELLO_WORLD.TXT";
    char* exfiledata = "HELLO WORLD\nTHIS IS AN EXAMPLE FILE!!!!\nYAY!!!!\n";
    uint8_t s = 49;
    write_file(tfile_name, exfiledata, s);*/
}

void user_input(char *input) {
    kprint("\n");
    resolve_command(*command_resolver_head, str_split(key_buffer, ' ')[0])(key_buffer);
    kprint("> ");

    key_buffer[0] = '\0';
}

void kernel_init_keyboard() {
    lkeybuffer = malloc(sizeof(char)*256);
    uint8_t *keycodes = malloc(sizeof(uint8_t)*3); // memory leak?
    keycodes[0] = 0x1C; keycodes[1] = 0x0; keycodes[2] = 0x0;
    void (**gcallback_functions)() = malloc(sizeof(void*)*10); // memory leak?
    *gcallback_functions = user_input;
    struct keyboard_initializer* keyboardi = create_initializer(lkeybuffer,
                                                                1,
                                                                keycodes,
                                                                gcallback_functions,
                                                                0x0);
    init_keyboard(keyboardi);
}