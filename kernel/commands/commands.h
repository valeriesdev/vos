#include <stdint.h>
void END(char *args);
void PAGE(char *args);
void ECHO(char *args); 
void NULLFUNC(char *args);

struct command_block {
	void (*function)();
	char *call_string;
	struct command_block *next;
};
void register_command(struct command_block *command_resolver_head, void (*function)(), char* function_call_string);
void (*resolve_command(struct command_block command_resolver_head, char* function_call_string))(char*);