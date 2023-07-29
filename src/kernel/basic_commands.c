/**
 * @defgroup   BASIC_COMMANDS basic commands
 *
 * @brief      This file holds the basic kernel commands.
 * 
 * @note       Commands should pass in a char *args, regardless of whether or not they use it. If not, use UNUSED(args) in the function.
 *
 * @author     A
 * @date       2023
 */
#include <stdint.h>
#include <stddef.h>
#include "kernel/commands.h"
#include "libc/string.h"
#include "libc/mem.h"
#include "drivers/screen.h"
#include "libc/function.h"
#include "filesystem/filesystem.h"
extern struct command_block *command_resolver_head;

void ECHO(char *args) {
	kprint(args);
	kprint("\n");
	//kprint(str_split(args,' ')[1]);
	//kprint("\n");

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
    kprintn(hex_to_ascii((int)page));

    UNUSED(args);
}

void LS(char *args) {
	//kprint("\n");
	struct file* files = get_files()+1;
	while(files->magic == 0xFFFFFFFF) {
		kprintn(files->name);
		files++;
	}

	UNUSED(args);
}

void HELP(char *args) {
	struct command_block t = *(command_resolver_head->next);
	while(t.next != NULL) {
		kprintn(t.call_string);
		t = *t.next;
	}
}