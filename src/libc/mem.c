/**
 * @defgroup   MEM memory
 *
 * @brief      This file implements a memory management system.
 * 
 * @par
 * The only functions which should be used elsewhere are
 * 		- memory_copy
 * 		- memory_set
 * 		- malloc
 * 		- free
 * 		- initialize_memory
 * @note       initialize_memory should be called in the kernel initialization process 
 * 
 * @author     Valerie Whitmire
 * @date       2023
 */
#include <stdint.h>
#include <stddef.h>
#include "libc/mem.h"
#include "libc/string.h"
#include "drivers/screen.h"
#define TRUE 1
#define FALSE 0

/**
 * @brief      Copys memory from source to dest
 *
 * @param      source  The source
 * @param      dest    The destination
 * @param[in]  nbytes  The number of bytes
 */
void memory_copy(uint8_t *source, uint8_t *dest, int nbytes) {
    int i;
    for (i = 0; i < nbytes; i++) {
        *(dest + i) = *(source + i);
    }
}

/**
 * @brief      Sets memory to a vlue
 *
 * @param      dest  The destination to be set
 * @param[in]  val   The value to set the bytes to
 * @param[in]  len   The amount of bytes to set
 */
void memory_set(uint8_t *dest, uint8_t val, uint32_t len) {
    uint8_t *temp = (uint8_t *)dest;
    for ( ; len != 0; len--) *temp++ = val;
}
/**
 * @brief      A block of memory.
 */
struct block {
	size_t size;
	struct block *next;
	uint8_t used;
	uint8_t valid;
	uint8_t data;
};

struct block *head    = (struct block*)0x10000;
struct block *top     = (struct block*)0x10000;

inline size_t align(size_t n) {
  return sizeof(struct block) + n;
}

void print_node(struct block *current) {
	/*kprint("ADDR: ");
	kprint(hex_to_ascii((int) current));
	kprint(", SIZE:");
	kprint(hex_to_ascii((int)(*current).size));
	kprint(", FREE:");
	kprint(hex_to_ascii((int)(*current).used));
	kprint(", NEXT:");
	kprint(hex_to_ascii((int)(*current).next));
	kprint("\n");*/
}

/**
 * @brief      Initializes the memory.
 */
void initialize_memory() {
	head = (struct block*)0x10000;
	top  = (struct block*)0x10000;
	(*top).size = align(32);
	(*top).next = NULL;
	(*top).valid = 7;
	(*top).used = TRUE;
	if((int)head == 0x10000) {
		kprintn("Memory initialized properly at 0x10000");
	} else {
		kprintn("MEMORY FAILED TO INITIALIZE!");
	}

	return;
}

/**
 * @brief      Allocates a block of memory
 *
 * @param[in]  size  The size of the block
 *
 * @return     The pointer to the start of the memory within that block
 */
void *alloc(size_t size) {
	struct block *newBlock = (struct block *)find_free(size);
	if((*newBlock).next == NULL) { // Procedure for allocating a block at the end of the db
		char* tptr = (char*)newBlock;
		newBlock = (struct block*)(tptr + (*newBlock).size);
		(*newBlock).size = align(size);
		(*newBlock).next = NULL;
		(*newBlock).valid = 7;
		(*newBlock).used = TRUE;

		(*top).next = newBlock;
		top = (*top).next;
	
		return &(*newBlock).data;
	} else { // Procedure for allocating a block at the middle of the db
		if((*newBlock).size - align(size) > align(sizeof(char))) { // Split block
			struct block *insertBlock = (struct block*)newBlock + align(size);
			(*insertBlock).size = (*newBlock).size - align(size);
			(*insertBlock).next = (*newBlock).next;
			(*insertBlock).valid = 7;
			(*insertBlock).used = FALSE;
			
			(*newBlock).size = align(size);
			(*newBlock).next = insertBlock;
			(*newBlock).used = TRUE;
			return &(*newBlock).data;
		} else { // Fit into oversize block
			(*newBlock).used = TRUE;
			return &(*newBlock).data;
		}
	}

	return NULL;
}

void *find_free(size_t n) {
	struct block *current = head;
	for(; (*current).next != NULL; current = (*current).next) {
		if((*current).used == FALSE && align(n) < (*current).size) return current;
	}
	if((*current).used == FALSE && align(n) < (*current).size) return current;
	return current;
}

/**
 * @brief      Frees a block of memory
 *
 * @param      address  The address of the value to be freed
 */
void free(void *address) {
	struct block *changeBlock = (struct block *)address;
	char *tBlock = (char*)address;

	for(; (int) tBlock >= (int) head; --tBlock) {
		if((*(struct block *)tBlock).valid == 7) {
			changeBlock = (struct block *)tBlock;
			break;
		}
	}
	(*changeBlock).used = FALSE;
	memory_set(&(*changeBlock).data, 0, (*changeBlock).size - align(0));
}

void traverse() {
	struct block *current = head;
	for(; (*current).next != NULL; current = (*current).next) {
		print_node(current);
	}
	print_node(current);
	kprint("\n");
}

/**
 * @brief      Allocate a block of memory
 *
 * @param[in]  size  The size
 *
 * @return     The address of the block
 */
void *malloc(uint32_t size) {
	void* t = alloc(size);
	return t;
}