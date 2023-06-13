#include <stdint.h>
#include <stddef.h>
#include "filesystem/filesystem.h"
#include "drivers/ata.h"
#include "drivers/screen.h"

struct file {
	char name[32];
	uint32_t lba;
	uint32_t length;
};

struct fat_node {
	struct file file;
	struct file* next;
};

// Load FAT from LBA65/0x8200
void load_fat_from_disk() {
	void *fat_location = malloc(sizeof(uint8_t)*6*512);
	read_sectors_ATA_PIO(fat_location, 65, 6);

	// processs FAT
}

void initialize_fat_to_disk() {
	
}