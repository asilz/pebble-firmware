/*
 * Copyright 2024 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "drivers/flash.h"
#include "drivers/flash/flash_impl.h"

status_t return_success(void) { return S_SUCCESS; }

status_t return_error(void) { return E_ERROR; }

status_t flash_impl_init(bool coredump_mode) { return S_SUCCESS; }

void flash_impl_use(void) {}
void flash_impl_release_many(uint32_t num_locks) {}

int get_subsector_base_calls = 0;
FlashAddress flash_impl_get_subsector_base_address(FlashAddress addr) {
  get_subsector_base_calls++;
  return addr & 0xffffff00;
}

int get_sector_base_calls = 0;
FlashAddress flash_impl_get_sector_base_address(FlashAddress addr) {
  get_sector_base_calls++;
  return addr & 0xfffff000;
}

int erase_subsector_begin_calls = 0;
status_t erase_subsector_begin_return = S_SUCCESS;
status_t flash_impl_erase_subsector_begin(FlashAddress addr) {
  erase_subsector_begin_calls++;
  return erase_subsector_begin_return;
}

int erase_sector_begin_calls = 0;
status_t erase_sector_begin_return = S_SUCCESS;
status_t flash_impl_erase_sector_begin(FlashAddress addr) {
  erase_sector_begin_calls++;
  return erase_sector_begin_return;
}

int get_erase_status_calls = 0;
status_t (*get_erase_status_fn)(void) = return_success;
status_t flash_impl_get_erase_status(void) {
  get_erase_status_calls++;
  return get_erase_status_fn();
}

int blank_check_subsector_calls = 0;
status_t blank_check_subsector_return = S_FALSE;
status_t flash_impl_blank_check_subsector(FlashAddress addr) {
  blank_check_subsector_calls++;
  return blank_check_subsector_return;
}

int blank_check_sector_calls = 0;
status_t blank_check_sector_return = S_FALSE;
status_t flash_impl_blank_check_sector(FlashAddress addr) {
  blank_check_sector_calls++;
  return blank_check_sector_return;
}

status_t flash_impl_enter_low_power_mode(void) { return S_SUCCESS; }

status_t flash_impl_exit_low_power_mode(void) { return S_SUCCESS; }

status_t flash_impl_erase_suspend(FlashAddress addr) { return S_SUCCESS; }

status_t flash_impl_erase_resume(FlashAddress addr) { return S_SUCCESS; }

uint32_t flash_impl_get_typical_subsector_erase_duration_ms(void) { return 100; }

uint32_t flash_impl_get_typical_sector_erase_duration_ms(void) { return 100; }

status_t flash_impl_get_write_status(void) { return S_SUCCESS; }

status_t flash_impl_read_sync(void *buffer, FlashAddress addr, size_t len) { return E_UNKNOWN; }

status_t flash_impl_set_burst_mode(bool enable) { return E_UNKNOWN; }

status_t flash_impl_unprotect(void) { return S_SUCCESS; }

int flash_impl_write_page_begin(const void *buffer, FlashAddress addr, size_t len) { return len; }

void flash_impl_enable_write_protection(void) {}

status_t flash_impl_write_protect(FlashAddress start_sector, FlashAddress end_sector) {
  return S_SUCCESS;
}
