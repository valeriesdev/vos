#include <stdint.h>
#include <stddef.h>
#include "filesystem/filesystem.h"
#include "drivers/ata.h"
#include "drivers/screen.h"
#include "libc/mem.h"
#include "libc/string.h"

#define initial_node_name "INIT_NODE"
#define FAT_LBA 65
#define FIRST_DATA_LBA 75

/**
 * name: 32 bytes
 * lba : 4  bytes
 * len : 4  bytes
 * maj : 4  bytes
 * rep until maj != 0xFFFFFFFF
 */ 

struct file {
	char name[32];
	uint32_t lba;
	uint32_t length;  // In sectors
	uint32_t magic;
};

struct file *fat_head;
uint32_t num_registered_files;
uint32_t first_free_sector;

void fs_debug() {	
	kprintn(int_to_ascii(num_registered_files));
	struct file *current = fat_head;
	while(current->magic == 0xFFFFFFFF) {
		kprint(current->name);
		kprint(" | ");
		kprint(int_to_ascii(current->lba));
		kprint(" | ");
		kprint(int_to_ascii(current->length));
		kprint(" | ");
		kprintn(hex_to_ascii(current->magic));
		current++;
	}
}

void write_file(char* name, void *file_data, uint32_t size_bytes) {
	struct file *node = fat_head+num_registered_files;
	kprintn(hex_to_ascii(node));
	uint32_t size_sectors = size_bytes/512;
	if(size_sectors == 0) size_sectors = 1;

	void* tfile = malloc(size_sectors*512);
	memory_copy((uint8_t*)file_data, (uint8_t*)tfile, size_bytes);

	memory_copy((uint8_t*)name, (uint8_t*)&(node->name), strlen(name)+1);
	node->length = size_sectors;
	node->lba = first_free_sector;
	node->magic = 0xFFFFFFFF;
	first_free_sector += size_sectors;

	write_sectors_ATA_PIO(node->lba, size_sectors ,(uint16_t*)tfile);	
	num_registered_files++;
}

void *read_file(char* name) {
	return NULL;
} 

// Load FAT from LBA80
void load_fat_from_disk() {
	fat_head = malloc(sizeof(uint8_t)*6*512);
	read_sectors_ATA_PIO((uint32_t)fat_head, FAT_LBA, 6);

	if(fat_head->magic != 0xFFFFFFFF) {
		kprintn("Loading FAT from disk failed, invalid allocation table. Creating new FAT");
		initialize_empty_fat_to_disk();
		free(fat_head);
		load_fat_from_disk();
	} else {
		kprintn("Successfully loaded FAT");
		num_registered_files = 1;
		first_free_sector = fat_head->lba+1;
	}
}

void initialize_fat_in_memory() {
	fat_head = (struct file*)malloc(sizeof(struct file));
	memory_copy((uint8_t*)&initial_node_name, (uint8_t*)&(fat_head->name), 9);
	fat_head->lba = FIRST_DATA_LBA;
	fat_head->length = 1;
	fat_head->magic = 0xFFFFFFFF;
}

void initialize_empty_fat_to_disk() {
	initialize_fat_in_memory();
	uint16_t* t_storage = malloc(sizeof(uint8_t)*32);
	memory_copy((uint8_t*)fat_head, (uint8_t*)t_storage,sizeof(fat_head));
	write_sectors_ATA_PIO(FAT_LBA, 2, (uint16_t*)fat_head);
}