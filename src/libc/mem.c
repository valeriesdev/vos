#include <stdint.h>
#include <stddef.h>
#include "libc/mem.h"
#include "libc/string.h"
#include "drivers/screen.h"
#define TRUE 1
#define FALSE 0

void memory_copy(uint8_t *source, uint8_t *dest, int nbytes) {
    int i;
    for (i = 0; i < nbytes; i++) {
        *(dest + i) = *(source + i);
    }
}

void memory_set(uint8_t *dest, uint8_t val, uint32_t len) {
    uint8_t *temp = (uint8_t *)dest;
    for ( ; len != 0; len--) *temp++ = val;
}

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

void initialize_memory() {
	head = (struct block*)0x10000;
	top  = (struct block*)0x10000;
	(*top).size = align(32);
	(*top).next = NULL;
	(*top).valid = 7;
	(*top).used = TRUE;
	kprintn(hex_to_ascii((int)head));
	print_node(head);

	return;
}

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

void *malloc(uint32_t size) {
	void* t = alloc(size);
	return t;
}