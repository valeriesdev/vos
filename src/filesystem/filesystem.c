#include <stdint.h>
#include <stddef.h>
#include "filesystem/filesystem.h"
#include "drivers/ata.h"
#include "drivers/screen.h"
#include "libc/mem.h"
#include "libc/string.h"

#define initial_node_name "INIT_NODE"

struct file {
	char name[32];
	uint32_t lba;
	uint32_t length;  // In sectors
	struct file* next;
};

struct file *fat_head;

void initialize_fat_in_memory() {
	fat_head = (struct file*)malloc(sizeof(struct file));
	memory_copy((uint8_t*)&initial_node_name, (uint8_t*)&(fat_head->name), 9);
	fat_head->lba = 74;
	fat_head->length = 1;
	fat_head->next = NULL;
}

// Load FAT from LBA65/0x8200
void load_fat_from_disk() {
	void *fat_location = malloc(sizeof(uint8_t)*6*512);
	read_sectors_ATA_PIO((uint32_t)fat_location, 65, 6);

	// processs FAT
}

void initialize_empty_fat_to_disk() {
	initialize_fat_in_memory();
	uint16_t* t_storage = malloc(sizeof(uint8_t)*32);
	memory_copy((uint8_t*)fat_head, (uint8_t*)t_storage,sizeof(fat_head));
	write_sectors_ATA_PIO(65, 2, (uint16_t*)fat_head);
}