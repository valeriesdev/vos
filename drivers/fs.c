#include <stdint.h>
#include "ata.h"
#include "../libc/mem.h"

uint32_t resolve_filename(char* name) {

	return 33;
}

uint32_t resolve_filesize(char* name) {
	
	return 1;
}

/*
 * Load program procedure:
 * Resolve LBA from filename via TBD procedures
 * Resolve file size via TBD procedures
 * Malloc file size into char*
 * Load sectors into char*
*/

void* load_program(char* name) {
	char* file_loaded = (char *)malloc(resolve_filesize(name) * 512);
	read_sectors_ATA_PIO(
		&file_loaded,
		resolve_filename(name), 
		resolve_filesize(name)
	);

	return file_loaded;
}