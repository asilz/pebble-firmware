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

#include "gap_le.h"

#include "comm/bt_lock.h"
#include "gap_le_advert.h"
#include "gap_le_connect.h"
#include "gap_le_connection.h"
#include "gap_le_scan.h"
#include "gap_le_slave_discovery.h"
#include "kernel_le_client/kernel_le_client.h"

void gap_le_init(void) {
  bt_lock();
  {
    gap_le_connection_init();
    gap_le_scan_init();
    gap_le_advert_init();
    gap_le_slave_discovery_init();
    // Depends on gap_le_advert:
    gap_le_connect_init();

    // kernel_le_client_init();
  }
  bt_unlock();
}

void gap_le_deinit(void) {
  bt_lock();
  {
    kernel_le_client_deinit();

    gap_le_connect_deinit();
    gap_le_slave_discovery_deinit();
    gap_le_advert_deinit();
    gap_le_scan_deinit();
    gap_le_connection_deinit();
  }
  bt_unlock();
}
