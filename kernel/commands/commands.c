#include "commands.h"
#include "../../libc/string.h"
#include <stdint.h>
#include <stddef.h>
void NULLFUNC(char* args) { return; }

void register_command(struct command_block *command_resolver_head, void (*new_function)(), char* function_call_string) {
	struct command_block *current = command_resolver_head;
	while(current->next != NULL) {
		current = current->next;
	}

	struct command_block *command_block_new = malloc(sizeof(struct command_block));
	*command_block_new = (struct command_block) {.function = new_function, .call_string = function_call_string, .next = NULL};
	current->next = command_block_new;
}

void (*resolve_command(struct command_block command_resolver_head, char* function_call_string))(char*) {
	struct command_block current = command_resolver_head;
	while(strcmp(current.call_string, function_call_string) != 0 && current.next != NULL) {
		current = *current.next;
	}

	if(strcmp(current.call_string, function_call_string) != 0) return NULLFUNC;
	return current.function;
}