/**
 * @defgroup   FILESYSTEM filesystem
 *
 * @brief      This file implements a FAT filesystem.
 * 
 * @par 
 * Read and write from the filesystem using the read_file and write_file functions. The FAT table must be set up using load_fat_from_disk before reading and writing. 
 * 
 * @note       A prior version of the tedit stock program caused major issues, possible due to issues in either the filesystem or ATA driver.
 * 
 * @author     Valerie Whitmire
 * @date       2023
 */
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

// Private function definitions
static void initialize_empty_fat_to_disk();
static void update_disk_fat();
static uint8_t get_file(char* name);

struct file *fat_head;
uint32_t num_registered_files;
uint32_t first_free_sector;

/**
 * @brief      Gets a file.
 * @ingroup    FILESYSTEM
 * @param      name  The name of the file
 *	
 * @return     The file index.
 */
static uint8_t get_file(char* name) {
	struct file *current = fat_head;
	while(current->magic == 0xFFFFFFFF) {
		if(strcmp(current->name, name) == 0) return 1;
		current++;
	}
	return 0;
}

/**
 * @brief      Writes a file.
 * @ingroup    FILESYSTEM
 * @param      name        The name of the file
 * @param      file_data   The file data
 * @param[in]  size_bytes  The size of the file, in bytes
 */
void write_file(char* name, void *file_data, uint32_t size_bytes) {
	if(get_file(name) != 0) return;

	struct file *node = fat_head+num_registered_files; // needs updating
	uint32_t size_sectors = size_bytes/512;
	if(size_sectors == 0) size_sectors = 1;

	memory_copy((uint8_t*)name, (uint8_t*)&(node->name), strlen(name)+1);
	node->length = size_sectors;
	node->lba = first_free_sector;
	node->magic = 0xFFFFFFFF;
	first_free_sector += size_sectors;

	write_sectors_ATA_PIO(node->lba, size_sectors ,(uint16_t*)file_data);	
	num_registered_files++;
	update_disk_fat();
}

/**
 * @brief      Overwrites an existing file
 * @ingroup    FILESYSTEM
 * @param      name        The name
 * @param      file_data   The file data
 * @param[in]  size_bytes  The size bytes
 * 
 * @todo       Add procedure to delete and re-create the file if it is too big.
 */
void overwrite_file(char* name, void *file_data, uint32_t size_bytes) {
	uint8_t file_number = get_file(name);
	if(file_number == 0) return;

	struct file *node = fat_head+file_number;
	uint32_t size_sectors = size_bytes/512;
	if(size_sectors == 0) size_sectors = 1;
	if(size_sectors <= node->length) {
		memory_copy((uint8_t*)name, (uint8_t*)&(node->name), strlen(name)+1);
		node->magic = 0xFFFFFFFF;

		write_sectors_ATA_PIO(node->lba, size_sectors ,(uint16_t*)file_data);	
	} else {
		// Delete and re-create
	}
	update_disk_fat();
}

/**
 * @brief      Reads a file.
 * @ingroup    FILESYSTEM
 * @param      name  The name of the file
 *
 * @return     A void pointer to that file in memory. <b>Must be free'd</b>
 */
void *read_file(char* name) {
	uint8_t offset = get_file(name);
	if(offset == 0) return NULL;
	struct file *file_to_read = fat_head+offset;
	void *return_file = malloc(file_to_read->length*512);
	read_sectors_ATA_PIO((uint32_t)return_file, file_to_read->lba, file_to_read->length);
	return return_file;
} 

/**
 * @brief      Loads a FAT table from disk.
 * @ingroup    FILESYSTEM
 * @note       If no FAT table is present, it will attempt to initialize one.
 */
void load_fat_from_disk() {
	fat_head = malloc(sizeof(uint8_t)*6*512); // Free handled
	read_sectors_ATA_PIO((uint32_t)fat_head, FAT_LBA, 6);

	if(fat_head->magic != 0xFFFFFFFF) {
		kprintn("Loading FAT from disk failed, invalid allocation table. Creating new FAT");
		initialize_empty_fat_to_disk();
		fat_head = free(fat_head);
		load_fat_from_disk();
	} else {
		kprintn("Successfully loaded FAT");
		num_registered_files = 1;
		first_free_sector = fat_head->lba+1;
	}
}

struct fat_code {
    uint32_t magic[4];
    char name[32];
    uint32_t lba;
    uint32_t length;
} __attribute__((packed));

void find_program_header() {
    int i = 0;
    for(i = 0; i < 256; i++) {
        //void* program = malloc((uint32_t)length_linker);
        void* program = malloc(512);
        //uint32_t z = ((uint32_t)&address_linker)/512;

        read_sectors_ATA_PIO(program, i*8, 1);
        if(((struct fat_code*) program)->magic[0] == 0xFFFFFFFF &&
           ((struct fat_code*) program)->magic[1] == 0xFFFFFFFF &&
           ((struct fat_code*) program)->magic[2] == 0xFFFFFFFF &&
           ((struct fat_code*) program)->magic[3] == 0xFFFFFFFF) {
            //i++;
        }
        free(program);
    }
}

/**
 * @brief      Updates the FAT table to disk
 * @ingroup    FILESYSTEM
 */
static void update_disk_fat() {
	uint8_t num_sectors = (sizeof(struct file)*num_registered_files/512 > 1 ) ? sizeof(struct file)*num_registered_files/512 : 1;
	uint16_t* t_storage = malloc(num_sectors*512);
	memory_copy((uint8_t*)fat_head, (uint8_t*)t_storage,sizeof(struct file)*num_registered_files);
	write_sectors_ATA_PIO(FAT_LBA, num_sectors, (uint16_t*)fat_head);
	t_storage = free(t_storage); // major source of memory leaking
}

/**
 * @brief      Initializes the empty fat table to disk.
 * @ingroup    FILESYSTEM
 * @todo       Fix fat_head and t_storage free behavior
 * @todo       Make static
 */
static void initialize_empty_fat_to_disk() {
	//fat_head = (struct file*)malloc(sizeof(struct file));
	fat_head = (struct file*)malloc(512*3);
	memory_copy((uint8_t*)&initial_node_name, (uint8_t*)&(fat_head->name), 9);
	fat_head->lba = FIRST_DATA_LBA;
	fat_head->length = 1;
	fat_head->magic = 0xFFFFFFFF;

	uint16_t* t_storage = malloc(512*3);
	memory_copy((uint8_t*)fat_head, (uint8_t*)t_storage,sizeof(fat_head));
	write_sectors_ATA_PIO(FAT_LBA, 2, (uint16_t*)fat_head);
}

/**
 * @brief      Gets the file list.
 * @ingroup    FILESYSTEM
 * @return     The file list.
 */
struct file *get_files() {
	return fat_head;
} 