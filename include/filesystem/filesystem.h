void load_fat_from_disk();
void initialize_empty_fat_to_disk();
void write_file(char* name, void *file_data, uint32_t size_bytes);
void fs_debug();