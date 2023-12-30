#include <stdint.h>

void enable_paging();
void set_page_present(uint32_t page_address);
void set_page_absent(uint32_t page_address);
uint32_t find_first_frame();
void set_page_value(uint32_t page_address, uint32_t page_value);

uint32_t *page_directory;
uint32_t *page_tables;

typedef struct page {
    uint32_t present  : 1;  // page present
    uint32_t rw       : 1;  // read/write or read only
    uint32_t user     : 1;  // user mode level
    uint32_t accessed : 1;
    uint32_t dirty    : 1;
    uint32_t unused   : 7;  // unused and reserved bitys
    uint32_t frame    : 20; // frame address
} page_t;

typedef struct page_table {
    page_t pages[1024];
} page_table_t;

typedef struct page_directory {
    page_table_t *tables[1024];
    uint32_t tables_physical[1024];
    uint32_t phyiscal_address;
} page_directory_t;

#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))