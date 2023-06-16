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

struct command_block command_resolver_head;

void kernel_main() {
    initialize_memory();

    isr_install();
    irq_install();

    kprint("Welcome to VOS!\n> ");

    struct command_block temporary_head = {NULLFUNC, "", NULL};
    command_resolver_head = temporary_head;

    register_command(&command_resolver_head, END, "END");
    register_command(&command_resolver_head, PAGE, "PAGE");
    register_command(&command_resolver_head, ECHO, "ECHO");
    register_command(&command_resolver_head, launch_tedit, "tedit"); 

    initialize_empty_fat_to_disk();
    load_fat_from_disk();
}


void user_input(char *input) {
    kprint("\n");
    resolve_command(command_resolver_head, str_split(key_buffer, ' ')[0])(key_buffer);
    kprint("> ");

    key_buffer[0] = '\0';
}