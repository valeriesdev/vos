#include <stdint.h>
#include "drivers/screen.h"

extern uint32_t address_linker;
extern uint32_t length_linker;

struct fat_code {
	uint32_t magic[4];
	char name[32];
	uint32_t lba;
	uint32_t length;
} __attribute__((packed));



__attribute__((section(".program_header"))) __attribute__((packed)) struct fat_code file_info = {
	.name = "test_program.vxv\0             \0",
	.lba = &address_linker,
	.length = &length_linker,
	.magic = {0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF}
};

__attribute__((section(".file_functions"))) void func()  {
	kprint("woah... this is a new program.\n I think im loaded at 0xF000000...\n but i'm actually loaded at 0x3003000");
	while(1);
}