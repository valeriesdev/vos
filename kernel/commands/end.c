#include "../../drivers/screen.h"
#include "commands.h"

void END(char *args) {
	kprint("Stopping the CPU. Bye!\n");
    asm volatile("hlt");
}