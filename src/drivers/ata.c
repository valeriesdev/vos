#include <stdint.h>
#include "cpu/ports.h"
#include "drivers/ata.h"

/*
BSY: a 1 means that the controller is busy executing a command. No register should be accessed (except the digital output register) while this bit is set.
RDY: a 1 means that the controller is ready to accept a command, and the drive is spinning at correct speed..
WFT: a 1 means that the controller detected a write fault.
SKC: a 1 means that the read/write head is in position (seek completed).
DRQ: a 1 means that the controller is expecting data (for a write) or is sending data (for a read). Don't access the data register while this bit is 0.
COR: a 1 indicates that the controller had to correct data, by using the ECC bytes (error correction code: extra bytes at the end of the sector that allows to verify its integrity and, sometimes, to correct errors).
IDX: a 1 indicates the the controller retected the index mark (which is not a hole on hard-drives).
ERR: a 1 indicates that an error occured. An error code has been placed in the error register.
*/

#define STATUS_BSY 0x80
#define STATUS_RDY 0x40
#define STATUS_DRQ 0x08
#define STATUS_DF 0x20
#define STATUS_ERR 0x01

#define ATA_DATA           0x1F0
#define ATA_ERROR          0x1F1
#define ATA_SECTOR_COUNT   0x1F2
#define ATA_LBA_LOW        0x1F3
#define ATA_LBA_MID        0x1F4
#define ATA_LBA_HIGH       0x1F5
#define ATA_SELECT_DRIVE   0x1F6
#define ATA_STATUS_COMMAND 0x1F7

#define ATA_READ_SECTORS   0x20
#define ATA_WRITE_SECTORS  0x30

static void ATA_wait_BSY();
static void ATA_wait_DRQ();

/**
 * Function: read_sectors_ATA_PIO
 * ------------------------------
 * Reads sector_count sectors from LBA into target address via ATA_PIO
 * 
 * target_address: address to store data in
 * LBA           : logical block address to read from
 * sector_count  : how many sectors to read
 * 
 * Returns       : nothing
 **/
void read_sectors_ATA_PIO(uint32_t target_address, uint32_t LBA, uint8_t sector_count) {

    ATA_wait_BSY();
    port_byte_out(ATA_SELECT_DRIVE,   0xE0 | ((LBA >>24) & 0xF));
    port_byte_out(ATA_SECTOR_COUNT,   sector_count);
    port_byte_out(ATA_LBA_LOW,        (uint8_t) LBA);
    port_byte_out(ATA_LBA_MID,        (uint8_t)(LBA >> 8));
    port_byte_out(ATA_LBA_HIGH,       (uint8_t)(LBA >> 16)); 
    port_byte_out(ATA_STATUS_COMMAND, ATA_READ_SECTORS);

    uint16_t *target = (uint16_t*) target_address;

    int j = 0;
    for (j = 0;j<sector_count;j++) {
        ATA_wait_BSY();
        ATA_wait_DRQ();
        int i = 0;
        for(i = 0;i < 256; i++) {
            target[i] = port_word_in(ATA_DATA);
        }
        target += 256;
    }
}

void write_sectors_ATA_PIO(uint32_t LBA, uint8_t sector_count, uint16_t* bytes) {
    ATA_wait_BSY();
    port_byte_out(ATA_SELECT_DRIVE,   0xE0 | ((LBA >>24) & 0xF));
    port_byte_out(ATA_SECTOR_COUNT,   sector_count);
    port_byte_out(ATA_LBA_LOW,        (uint8_t) LBA);
    port_byte_out(ATA_LBA_MID,        (uint8_t)(LBA >> 8));
    port_byte_out(ATA_LBA_HIGH,       (uint8_t)(LBA >> 16)); 
    port_byte_out(ATA_STATUS_COMMAND, ATA_WRITE_SECTORS);

    int j = 0;
    for (j = 0; j < sector_count; j++) {
        ATA_wait_BSY();
        ATA_wait_DRQ();
        int i = 0;
        for(i = 0; i < 256; i++) {
            port_word_out(ATA_DATA, bytes[i]);
        }
    }
}

// Loops until ATA status is no longer busy
static void ATA_wait_BSY() {
    while(port_byte_in(ATA_STATUS_COMMAND)&STATUS_BSY);
}

// Loops until ATA status is ready
static void ATA_wait_DRQ() {
    while(!(port_byte_in(ATA_STATUS_COMMAND)&STATUS_RDY));
}