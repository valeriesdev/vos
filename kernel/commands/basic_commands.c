#include "commands.h"
#include "../../libc/string.h"
#include "../../libc/mem.h"
#include "../../drivers/screen.h"
#include <stdint.h>
#include <stddef.h>

void ECHO(char *args) {
	kprint(args);
	kprint("\n");
	kprint(str_split("ECHO HELLO",' ')[1]);
	kprint("\n");
	return;
}

void END(char *args) {
	kprint("Stopping the CPU. Bye!\n");
    asm volatile("hlt");
}

void PAGE(char *args) {
    kprintn(args);
    uint32_t phys_addr;
    uint32_t page = malloc(1000);
    kprint("Page: ");
    kprintn(hex_to_ascii(page));
}