#include "libc/mem.h"

uint8_t enable_paging() {
	uint32_t *page_directory = malloc_align(4096, 4096);
	int i;
	//for(i = 0; i < 1024; i++) {
	    // This sets the following flags to the pages:
	    //   Supervisor: Only kernel-mode can access them
	    //   Write Enabled: It can be both read from and written to
	    //   Not Present: The page table is not present
	//    page_directory[i] = 0x00000002;
	//}

	for(i = 0; i < 1024; ++i) {
		uint32_t *page_table = malloc_align(4096, 4096);
		int j = 0;
		for(j = 0; j < 1024; j++) {
			page_table[j] = (i*4096*1024 + j * 0x1000) | 3;
		}
		page_directory[i] = ((unsigned int)page_table) | 3;
	}



	register uint32_t *page_directory_reference asm("eax");
	page_directory_reference = page_directory;
	// load page directory
	__asm__("mov %eax, %cr3\n\t");
	// enable paging
	__asm__("mov %cr0, %ebx\n\t"
		    "or $0x80000000, %ebx\n\t"
		    "mov %ebx, %cr0\n\t"
			);
	// flush the tlb
	__asm__("mov %cr3, %eax\n\t"
		    "mov %eax, %cr3\n\t");


	return 0;
}