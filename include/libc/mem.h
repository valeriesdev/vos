#include <stdint.h>
#include <stddef.h>

void memory_copy(uint8_t *source, uint8_t *dest, int nbytes);
void memory_set(uint8_t *dest, uint8_t val, uint32_t len);
void *kmalloc(uint32_t size);
void initialize_memory();
void free(uint32_t address);
void *find_free(size_t n);
void traverse();