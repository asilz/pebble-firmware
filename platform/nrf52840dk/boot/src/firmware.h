#pragma once

extern char __fw_start[];
#define FIRMWARE_BASE ((uint32_t)(__fw_start))
