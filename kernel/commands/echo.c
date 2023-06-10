#include "commands.h"
#include "../../libc/string.h"
void ECHO(char *args) {
	kprint(args);
	kprint("\n");
	kprint(str_split("ECHO HELLO",' ')[1]);
	kprint("\n");
	return;
}