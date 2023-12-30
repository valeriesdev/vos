/**
 * @defgroup   KERNEL kernel
 * @ingroup    KERNEL_FILES
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

#include "cpu/paging.h"

#include "cpu/ports.h"

#include "cpu/task_handler.h"


struct command_block *command_resolver_head;
char **lkeybuffer = NULL;

vf_ptr_s next_function = NULL;

/**
 * @brief      The kernel entry point.
 * @ingroup    KERNEL
 */
void kernel_main() {
    kprint("Initializing memory manager.\n");
    initialize_memory();
    kprint("Installing ISR.\n");
    isr_install();
    kprint("ISR Installed.\nInstalling IRQ.\n");
    irq_install();
    kprint("IRQ Installed.\n");


    kprint("Enabling paging. This might take a while...\n");
    enable_paging();
    kprint("Paging enabled.\nLoading FAT from disk.\n");

    load_fat_from_disk();

    kprint("Welcome to VOS!\n> ");

    //uint8_t* page_fault_test = (uint8_t*)(0x3003000);
    //set_page_present(0x3003000);
    //(*page_fault_test)++;

    command_resolver_head = malloc(sizeof(struct command_block)); // Does not need to be freed; should always stay in memory
    command_resolver_head->function = NULLFUNC;
    command_resolver_head->call_string = "";
    command_resolver_head->next = NULL;

    register_command(command_resolver_head, END, "end");
    register_command(command_resolver_head, PAGE, "page");
    register_command(command_resolver_head, ECHO, "echo");
    register_command(command_resolver_head, LS, "ls");
    register_command(command_resolver_head, HELP, "help");
    register_command(command_resolver_head, DEBUG_PAUSE, "debug_command");
    register_command(command_resolver_head, tedit, "tedit"); 
    //start_process(tedit);
    while(1) {
        if(next_function != NULL) {
            kprint("\n");
            char** args = str_split(get_keybuffer(),' ');
            char* args_processed = malloc(sizeof(char)*30);
            int current_arg = 1;
            while(args[current_arg] != 0x0 && args[current_arg] != '\0') {
                int i = 0;
                for(i = 0; i < strlen(args[current_arg]); i++) {
                    append(args_processed, args[current_arg][i]);
                }
                current_arg++;
                append(args_processed, ' ');
            }

            next_function(args_processed);
            next_function = NULL;
            kprint("> ");
            get_keybuffer()[0] = '\0';
            free(args_processed);
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