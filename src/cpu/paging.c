/**
 * @defgroup   PAGING paging
 * @ingroup    CPU 
 *
 * @brief      This file implements paging.
 * @todo       Rewrite
 *
 * @author     Valerie Whitmire
 * @date       2023
 */
#include "libc/mem.h"
#include "cpu/isr.h"
#include "drivers/screen.h"

void page_fault(registers_t *regs);

uint8_t enable_paging() {
	uint32_t *page_directory = malloc_align(4096, 4096);

	uint32_t *page_tables = malloc_align(4096*1024, 4096);
	int i = 0;
	for(i = 0; i < 1024; ++i) {
		int j = 0;
		for(j = 0; j < 1024; j++) {
			page_tables[1024*i+j] = (i*4096*1024 + j * 0x1000) | 3;
		}
		page_directory[i] = ((unsigned int)(page_tables + 1024*i)) | 3;
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

	register_interrupt_handler(46, page_fault);

	return 0;
}

void page_fault(registers_t *regs) {
   // A page fault has occurred.
   // The faulting address is stored in the CR2 register.
   uint32_t faulting_address;
   asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

   // The error code gives us details of what happened.
   int present   = !(regs->err_code & 0x1); // Page not present
   int rw = regs->err_code & 0x2;           // Write operation?
   int us = regs->err_code & 0x4;           // Processor was in user-mode?
   int reserved = regs->err_code & 0x8;     // Overwritten CPU-reserved bits of page entry?
   int id = regs->err_code & 0x10;          // Caused by an instruction fetch?

   // Output an error message.
   kprint("Page fault! ( ");
   if (present) {kprint("present ");}
   if (rw) {kprint("read-only ");}
   if (us) {kprint("user-mode ");}
   if (reserved) {kprint("reserved ");}
   kprint(") at 0x");
   //monitor_write_hex(faulting_address);
   kprint("\n");
   //PANIC("Page fault");
   while(1);
} 