#include "drivers/watchdog.h"

#include <helpers/nrfx_reset_reason.h>
#include <nrfx.h>

void watchdog_init(void) {
  return;
}

void watchdog_kick(void) {
  return;
}

bool watchdog_check_clear_reset_flag(void) {
  return false;
}
