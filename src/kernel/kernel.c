/**
 * @defgroup   KERNEL kernel
 *
 * @brief      The operating system kernel entry point
 * @todo       Replace get_keybuffer() calls with reference to local keybuffer
 * 
 * @author     Valerie Whitmire
 * @date       2023
 */
#include <stdint.h>
#include "cpu/isr.h"
#include "drivers/screen.h"
#include "drivers/ata.h"
#include "libc/function.h"
#include "libc/string.h"
#include "libc/mem.h"
#include "libc/vstddef.h"
#include "kernel/kernel.h"
#include "kernel/commands.h"
#include "stock/tedit/tedit.h"
#include "filesystem/filesystem.h"
#include "drivers/keyboard.h"
#include "cpu/timer.h"

struct command_block *command_resolver_head;
char **lkeybuffer;

vf_ptr_s next_function = NULL;

/**
 * @brief      The kernel entry point.
 * @ingroup    KERNEL
 */
void kernel_main() {
    initialize_memory();
    load_fat_from_disk();

    isr_install();
    irq_install();

    kprint("Welcome to VOS!\n> ");

    command_resolver_head = malloc(sizeof(struct command_block)); // Does not need to be freed; should always stay in memory
    command_resolver_head->function = NULLFUNC;
    command_resolver_head->call_string = "";
    command_resolver_head->next = NULL;

    register_command(command_resolver_head, END, "end");
    register_command(command_resolver_head, PAGE, "page");
    register_command(command_resolver_head, ECHO, "echo");
    register_command(command_resolver_head, LS, "ls");
    register_command(command_resolver_head, HELP, "help");
    register_command(command_resolver_head, DEBUG_PAUSE, "dbp");
    register_command(command_resolver_head, tedit, "tedit"); 

    /*char* tfile_name = "HELLO_WORLD.TXT";
    char* exfiledata = "HELLO WORLD\nTHIS IS AN EXAMPLE FILE!!!!\nYAY!!!!\n";
    uint8_t s = 49;
    write_file(tfile_name, exfiledata, s);*/
    while(1) {
        if(next_function != NULL) {
            kprint("\n");
            next_function(get_keybuffer());
            next_function = NULL;
            kprint("> ");
            get_keybuffer()[0] = '\0';
        }
    }
}

void user_input(char *input) {
    next_function = resolve_command(*command_resolver_head, str_split(get_keybuffer(), ' ')[0]);
    UNUSED(input);
}

/**
 * @brief      Initializes the keyboard to the state it should be in for CLI use
 * @ingroup    KERNEL
 */
void kernel_init_keyboard() {
    if(lkeybuffer != NULL) {
        lkeybuffer = free(lkeybuffer);
    }
    lkeybuffer = malloc(sizeof(char)*256);
    uint8_t keycodes[] = {0x1C, 0x0, 0x0};
    void (*gcallback_functions[])() = {user_input, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
    struct keyboard_initializer* keyboardi = create_initializer(1,
                                                                keycodes,
                                                                gcallback_functions,
                                                                0x0,
                                                                (char*) lkeybuffer); // ??? Why does casting from a char** to a char* just work???);
    init_keyboard(keyboardi);
}