#include "drivers/system_flash.h"

#include "drivers/dbgserial.h"
#include "drivers/watchdog.h"
#include "util/misc.h"

#define FLASH_SECTOR_SIZE 0x1000
#define FLASH_SECTOR_COUNT 256

bool system_flash_erase(uint32_t address, size_t length, SystemFlashProgressCb progress_callback,
                        void *progress_context) {
  return true;
}

bool system_flash_write(uint32_t address, const void *data, size_t length,
                        SystemFlashProgressCb progress_callback, void *progress_context) {
  return true;
}
