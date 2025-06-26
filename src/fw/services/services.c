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

#include "services.h"

#include <stdlib.h>
#include <string.h>

#include "console/prompt.h"
#include "runlevel.h"
#include "services/common/services_common.h"
#include "services/normal/services_normal.h"
#include "system/logging.h"
#include "system/passert.h"
#include "util/size.h"
#include "util/string.h"

void services_early_init(void) {
#ifndef RECOVERY_FW
  services_normal_early_init();
#endif
}

void services_init(void) {
  services_common_init();

#ifndef RECOVERY_FW
  // services_normal_init();
#endif
}

void services_set_runlevel(RunLevel runlevel) {
  PBL_ASSERT(runlevel < RunLevel_COUNT, "Unknown runlevel %d", runlevel);
  PBL_LOG(LOG_LEVEL_INFO, "Setting runlevel to %d", runlevel);
  services_common_set_runlevel(runlevel);
#ifndef RECOVERY_FW
  services_normal_set_runlevel(runlevel);
#endif
}

static const char *s_runlevel_debug_names[] = {
#define RUNLEVEL(number, name) [number] = #name,
#include "runlevel.def"
#undef RUNLEVEL
};

void prv_list_runlevels(void) {
  for (size_t i = 0; i < ARRAY_LENGTH(s_runlevel_debug_names); ++i) {
    char response[80];
    itoa_int(i, response, 10);
    strcat(response, " - ");
    strcat(response, s_runlevel_debug_names[i]);
    prompt_send_response(response);
  }
}

void command_set_runlevel(char *arg) {
  if (strcmp(arg, "list") == 0) {
    prv_list_runlevels();
    return;
  }

  int runlevel = atoi(arg);
  if (runlevel < 0 || runlevel >= RunLevel_COUNT) {
    prompt_send_response("Unknown runlevel");
    return;
  } else if (runlevel == 0 && arg[0] != '0') {
    prompt_send_response("Invalid runlevel number. Choices:");
    prv_list_runlevels();
    return;
  }

  char response[80];
  strcpy(response, "Switching to runlevel ");
  strcat(response, s_runlevel_debug_names[runlevel]);
  prompt_send_response(response);

  services_set_runlevel(runlevel);
}
