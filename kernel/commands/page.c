#include "../../libc/mem.h"
#include "../../libc/string.h"
#include "../../drivers/screen.h"
#include "commands.h"
#include <stdint.h>
#include <stddef.h>

void PAGE(char *args) {
        kprintn(args);
        uint32_t phys_addr;
        uint32_t page = malloc(1000);
        kprint("Page: ");
        kprintn(hex_to_ascii(page));
}