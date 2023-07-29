struct file {
	char name[32];
	uint32_t lba;
	uint32_t length;  // In sectors : needs to be updated to be in bytes, most likely
	uint32_t magic;
};

void load_fat_from_disk();
void initialize_empty_fat_to_disk();
void write_file(char* name, void *file_data, uint32_t size_bytes);
void *read_file(char* name);
struct file *get_files();
uint8_t get_file(char* name);