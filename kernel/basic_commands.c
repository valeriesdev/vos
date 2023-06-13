#include "commands.h"
#include "../libc/string.h"
#include "../libc/mem.h"
#include "../drivers/screen.h"
#include "../libc/function.h"
#include <stdint.h>
#include <stddef.h>

void ECHO(char *args) {
	kprint(args);
	kprint("\n");
	kprint(str_split("ECHO HELLO",' ')[1]);
	kprint("\n");

	UNUSED(args);

	return;
}

void END(char *args) {
	kprint("Stopping the CPU. Bye!\n");
    asm volatile("hlt");

    UNUSED(args);
}

void PAGE(char *args) {
    void* page = malloc(1000);
    kprint("Page: ");
    kprintn(hex_to_ascii(page));

    UNUSED(args);
}