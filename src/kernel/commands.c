#include <stdint.h>
#include <stddef.h>
#include "kernel/commands.h"
#include "libc/string.h"
void NULLFUNC(char* args) { return; }

/**
 * Function: register_command
 * --------------------------
 * Registers a new command into the linked list of command blocks.
 * 
 * *command_resolver_head: the head of the command linked list. (&command_resolver_head)
 * *new_function         : a pointer to the function that should be associated with this command
 * function_call_string  : the string that, when resolved, will point to the new function
 * 
 * Returns: nothing
 * 
 * Example syntax: register_command(&command_resolver_head, END, "END");
 *                 registers END to the string "END" in the command list command_resovler_head
 **/
void register_command(struct command_block *command_resolver_head, void (*new_function)(), char* function_call_string) {
	struct command_block *current = command_resolver_head;
	while(current->next != NULL) {
		current = current->next;
	}

	struct command_block *command_block_new = malloc(sizeof(struct command_block));
	*command_block_new = (struct command_block) {.function = new_function, .call_string = function_call_string, .next = NULL};
	current->next = command_block_new;
}

/**
 * Function: resolve_command
 * -------------------------
 * Resolves a string into a function pointer
 * 
 * command_resolver_head: the head of the command linked list.
 * function_call_string : the string which is being resolved.
 * 
 * Returns: a function pointer which takes in a char* and returns void.
 *          if string cannot be resolved, the NULLFUNC is returned.
 * 
 * Example syntax: resolve_command(command_resolver_head, str_split(input, ' ')[0])(input);
 *                 resolves the first word in string `input` to a function, which is called with
 *                 args `input`.
 **/
void (*resolve_command(struct command_block command_resolver_head, char* function_call_string))(char*) {
	struct command_block current = command_resolver_head;
	while(strcmp(current.call_string, function_call_string) != 0 && current.next != NULL) {
		current = *current.next;
	}

	if(strcmp(current.call_string, function_call_string) != 0) return NULLFUNC;
	return current.function;
}