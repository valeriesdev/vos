#include "cpu/paging.h"
#include "filesystem/filesystem.h"
#include "libc/mem.h"
#include <stdint.h>

void start_process(char *file_name) {
	// Grab an empty page
	// Identity map the page
	// Load program into that page
	// Demap that page
	// Duplicate the page directory
	// Remap your page(s) to 0xf00000
	// Load our new directory
	// 
	// 
	// Grab an empty page
	uint32_t free_page = find_first_frame();
	// Identity map the page
	set_page_value(free_page*0x1000,free_page*0x1000);
	set_page_present(free_page*0x1000);
	// Load program into that page
	void* process_file = read_file(file_name);
	memory_copy(process_file,(uint8_t*)(free_page*0x1000),4096);
	// Demap that page
	set_page_absent(free_page*0x1000);
	// Duplicate the page directory
	uint32_t* new_page_directory = malloc_align(4096, 4096);
	uint32_t* new_page_tables = malloc_align(1024*1024*4, 4096);
	memory_copy(page_directory, new_page_directory, 4096);
	memory_copy(page_tables, new_page_tables, 1024*1024*4);
	// Remap your page(s) to 0xf00000
	set_page_value(free_page*0x1000,0xF00000);
	// Load our new directory
	uint32_t eip, esp, ebp;
	eip = 0xF00000;

	__asm__("cli\n\t"
		    "mov %0, %%esp\n\t"
		    //"mov %1, %%ebp\n\t"
		    "mov %2, %%cr3\n\t"
		    "sti\n\t"
		    "mov %1, %%ecx\n\t"
		    "jmp %%ecx\n\t"
		    	: : "r"(esp), "r"(eip), /*"r"(ebp),*/ "r"(new_page_directory));
}