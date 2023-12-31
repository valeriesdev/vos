#include <stdint.h>

extern uint32_t address_linker;
extern uint32_t length_linker;

struct fat_code {
	uint32_t magic[4];
	char name[32];
	uint32_t lba;
	uint32_t length;
} __attribute__((packed));



__attribute__((section(".program_header"))) __attribute__((packed)) struct fat_code file_info = {
	.name = "test_program                  \0",
	.lba = &address_linker,
	.length = &length_linker,
	.magic = {0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF}
};

__attribute__((section(".file_functions"))) void func()  {
	func();
	while(1);
}