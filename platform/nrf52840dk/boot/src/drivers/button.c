#include "drivers/button.h"

#include <board.h>

#include <nrfx.h>

// TODO: Implement driver

bool button_is_pressed(ButtonId id)
{
	return false;
}

uint8_t button_get_state_bits(void)
{
	return 0x00;
}

void button_init(void)
{
	return;
}
