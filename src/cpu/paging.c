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
#include "drivers/screen.h"
#include "libc/string.h"
#include "cpu/isr.h"
#include "cpu/paging.h"

static char* h_to_a_inline(int n);
void page_fault(registers_t *regs);

void allocate_frame(page_t *page, uint8_t is_kernel, uint8_t is_writeable);
void free_frame(page_t *page);
static void set_frame(uint32_t frame_address);
static void clear_frame(uint32_t frame_address);
static void test_frame(uint32_t frame_address);
static uint32_t find_first_frame();

uint32_t *page_directory = NULL;
uint32_t *map = NULL;

page_directory_t kernel_directory = 0;
page_directory_t current_directory = 0;

// bitmap
uint32_t *frames;
uint32_t nframes;

/**
 * @brief      Enables paging
 * @ingroup    PAGING
 * 
 * @todo       Fix high address space paging.
 * 
 */
uint8_t enable_paging() {
    uint32_t end_page = 1024*1024*4*0x1000;         // the last memory address that will be pages

    nframes = end_page/0x1000;                      // the number of page frames
    frames = (uint32_t*)malloc(nframes);      // allocate the frame bitmap
    memory_set(frames, 0, INDEX_FROM_BIT(nframes)); // set all bitmap frames to 0

    kernel_directory = (page_directory_t*)malloc_align(sizeof(page_directory_t));
    memset(kernel_directory, 0, sizeof(page_directory_t));
    current_directory = kernel_directory;

    int i = 0;
    while(i <)


    /*
	page_directory = malloc_align(4096, 4096);
    map = malloc(32768); //each bit represents a page's "is occupied" bit

	uint32_t *page_tables = malloc_align(1024*1024*4, 4096);
	int i = 0, j = 0;
	for(i = 0; i < 1024; i++) {     // i corresponds to current page table
		for(j = 0; j < 1024; j++) { // j corresponds to current page
			page_tables[1024*i+j] = ((1024*i+j)*0x1000) | 3;
		}
		page_directory[i] = ((unsigned int)&page_tables[1024*i]) | 3;
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

	register_interrupt_handler(14, page_fault);

	return 0;*/
}

/**
 * @brief      Allocates a page frame
 *
 * @param      page          The page
 * @param[in]  is_kernel     Indicates if kernel
 * @param[in]  is_writeable  Indicates if writeable
 */
void allocate_frame(page_t *page, uint8_t is_kernel, uint8_t is_writeable) {
    if(page->frame != 0) return; // Return if frame was already allocated

    uint32_t index = find_first_frame();
    if(index == (uint32_t)-1) page_fault(NULL); // ERROR! no free frames
    set_frame(index*0x1000);
    page->present = 1;
    page->rw = (is_writeable)?1:0;
    page->user = (is_kernel)?0:1;
    page->frame = index;
}

/**
 * @brief      Frees a page frame
 * @todo       Free the memory within that page
 *
 * @param      page  The page
 */
void free_frame(page_t *page) {
    uint32_t frame = page->frame;
    clear_frame(frame);
    page->frame = 0x0;
}

/**
 * @brief      Sets a frame to be present in our bitmap
 * @ingroup    PAGING
 *
 * @param[in]  frame_address  The frame address
 */
static void set_frame(uint32_t frame_address) {
    uint32_t frame = frame_address/0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    frames[idx] |= (0x1 << off);
}

/**
 * @brief      Sets a frame to be not present in our bitmap
 * @ingroup    PAGING
 *
 * @param[in]  frame_address  The frame address
 */
static void clear_frame(uint32_t frame_address) {
    uint32_t frame = frame_address/0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    frames[idx] &= ~(0x1 << off);
}

/**
 * @brief      Checks if a frame is present in our bitmap
 * @ingroup    PAGING
 *
 * @param[in]  frame_address  The frame address
 */
static void test_frame(uint32_t frame_address) {
    uint32_t frame = frame_address/0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    return (frames[idx] & (0x1 << off));
}

/**
 * @brief      Finds the first free frame
 * @ingroup    PAGING
 *
 * @return     The address of the first free frame
 */
static uint32_t find_first_frame() {
    uint32_t i, j;
    for(i = 0; i < 1024*1024; i++) {
        if(frames[i] != 0xFFFFFFFF) {
            for(j = 0; j < 32; j++) {
                uint32_t current_bit = 0x1 << j;
                if(!(frames[i]&current_bit)) return i*4*8 + j;
            }
        }
    }

    return -1;
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
   kprint(") at ");
   kprint(h_to_a_inline(faulting_address));
   kprint("\n");
   //PANIC("Page fault");
   while(1);
} 

static char* h_to_a_inline(int n) {
    char *str = "0x0\0000000000000000000000";

    const char * hex = "0123456789abcdef";
    uint8_t blankspace = 1;

    int32_t tmp;
    int i;
    for (i = 28; i >= 0; i -= 4) {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && blankspace) continue;
        blankspace = 0;
        append(str, hex[tmp]);
    }
    
    return str;
}