#include "../../drivers/screen.h"
#include "../../drivers/keyboard.h"
#include "../../cpu/ports.h"
#include "../../cpu/isr.h"
#include "../../libc/string.h"
#include "../../libc/function.h"
#include "../../kernel/kernel.h"
#include "../../libc/mem.h"

extern char* key_buffer;
extern char* file;
void launch_tedit(char *args);
void file_writing_hook(registers_t *regs);
void init_special_keys();
void get_string_hook(registers_t *regs);
void filename_recieved();
void file_save();