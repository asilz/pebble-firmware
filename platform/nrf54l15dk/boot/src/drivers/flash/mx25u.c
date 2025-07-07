#include <board.h>
#include <drivers/dbgserial.h>
#include <drivers/flash.h>
#include <system/passert.h>

#define SPI_NOR_CMD_RDID 0x9F
#define SPI_NOR_CMD_RDPD 0xAB
#define SPI_NOR_CMD_ENRST 0x66
#define SPI_NOR_CMD_RST 0x99
#define SPI_NOR_CMD_EN4B 0xB7
#define SPI_NOR_CMD_RDSR1 0x05
#define SPI_NOR_CMD_RDSR2 0x35
#define SPI_NOR_CMD_WRSR 0x01

void flash_init(void) {}

bool flash_sanity_check(void) { return true; }

void flash_read_bytes(uint8_t *buffer_ptr, uint32_t start_addr, uint32_t buffer_size) { return; }
