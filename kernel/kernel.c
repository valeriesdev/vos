#include "../cpu/isr.h"
#include "../drivers/screen.h"
#include "kernel.h"
#include "../libc/string.h"
#include "../libc/mem.h"
#include <stdint.h>
#include "../drivers/ata.h"
#include "commands/commands.h"

struct command_block command_resolver_head;

void kernel_main() {
    isr_install();
    irq_install();

    initialize_memory();

    kprint("Type END to halt the CPU or PAGE to request a kmalloc()\n> ");

    struct command_block temporary_head = {NULLFUNC, "", NULL};
    command_resolver_head = temporary_head;

    register_command(&command_resolver_head, END, "END");
    register_command(&command_resolver_head, PAGE, "PAGE");
    register_command(&command_resolver_head, ECHO, "ECHO");
}


void user_input(char *input) {
    resolve_command(command_resolver_head, input)(input);
    kprint("You said: ");
    kprint(input);
    kprint("\n> ");
}