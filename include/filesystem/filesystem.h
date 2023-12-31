#ifndef FILESYSTEM_H
#define FILESYSTEM_H

struct file {
	char name[32];
	uint32_t lba;
	uint32_t length;  // In sectors : needs to be updated to be in bytes, most likely
	uint32_t magic;
	//uint8_t padding[128-(32+4+4+4+4)];
};

void load_fat_from_disk();
void write_file(char* name, void *file_data, uint32_t size_bytes);
void overwrite_file(char* name, void *file_data, uint32_t size_bytes);
void *read_file(char* name);
struct file *get_files();

#endif