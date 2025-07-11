#include "drivers/dbgserial.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <board.h>

#include <hal/nrf_pwm.h>
#include <nrfx_spim.h>

// Bootloader images
#include "drivers/display/resources/hex_digits.h"
#include "drivers/display/resources/dead_face.xbm"
#include "drivers/display/resources/empty_bar.xbm"
#include "drivers/display/resources/error_url.xbm"
#include "drivers/display/resources/pebbleos_logo.xbm"

#define DISP_COLS 144
#define DISP_ROWS 168
#define PBL_DISP_SHAPE_RECT

#define DISP_LINE_BYTES (DISP_COLS / 8)
#define DISP_LINE_WORDS (((DISP_COLS - 1) / 32) + 1)

// GPIO constants
#define DISP_MODE_STATIC (0x00)
#define DISP_MODE_WRITE (0x80)
#define DISP_MODE_CLEAR (0x20)

static uint16_t s_extcomin_period;

static nrfx_spim_t spim = NRFX_SPIM_INSTANCE(3);
static nrfx_spim_config_t config = NRFX_SPIM_DEFAULT_CONFIG(
	BOARD_DISP_SCK_PIN, BOARD_DISP_MOSI_PIN, NRF_SPIM_PIN_NOT_CONNECTED,
	NRF_SPIM_PIN_NOT_CONNECTED
);

static void prv_enable_chip_select(void) {
  //GPIO_WriteBit(DISP_GPIO, DISP_PIN_SCS, Bit_SET);
  BOARD_DISP_CS_PORT->OUTSET = (1 << BOARD_DISP_CS_PIN);
  // required setup time > 3us
  NRFX_DELAY_US(7);
}

static void prv_disable_chip_select(void) {
  // delay while last byte is emitted by the SPI peripheral
  NRFX_DELAY_US(7);

  //GPIO_WriteBit(DISP_GPIO, DISP_PIN_SCS, Bit_RESET);
  BOARD_DISP_CS_PORT->OUTCLR = (1 << BOARD_DISP_CS_PIN);
  // hold time > 1us
  // produce a delay 4ms
  NRFX_DELAY_US(4);
}

//! These functions needed to be called around any commands that
//! are sent to the display. NOINLINE only for code size savings.
static void prv_enable_display_access(void) {
  prv_enable_chip_select();
}

static void prv_disable_display_access(void) {
  prv_disable_chip_select();
}

//! Write a single byte synchronously to the display. This is the only practical
//! way to write to the display in the bootloader since we don't have interrupts.
static void prv_display_write_byte(uint8_t d) {
  nrfx_spim_xfer_desc_t desc = NRFX_SPIM_XFER_TX(&d, 1);
  nrfx_spim_xfer(&spim, &desc, 0);
  // Block until the tx buffer is empty
  //SPI_I2S_SendData(DISP_SPI, d);
  //while (!SPI_I2S_GetFlagStatus(DISP_SPI, SPI_I2S_FLAG_TXE)) {}
}

// Since all these values are constant we can save code space
// by storing the initialized struct in memory rather than
// needing to copy in each value
/*
static SPI_InitTypeDef s_disp_spi_init = {
  .SPI_Direction = SPI_Direction_1Line_Tx, // Write-only SPI
  .SPI_Mode = SPI_Mode_Master,
  .SPI_DataSize = SPI_DataSize_8b,
  .SPI_CPOL = SPI_CPOL_Low,
  .SPI_CPHA = SPI_CPHA_1Edge,
  .SPI_NSS = SPI_NSS_Soft,
  // We want the SPI clock to run at 2MHz
  .SPI_BaudRatePrescaler = DISPLAY_PERIPH_PRESCALER,
  // MSB order allows us to write pixels out without reversing bytes, but command bytes
  // have to be reversed
  .SPI_FirstBit = SPI_FirstBit_MSB,
  .SPI_CRCPolynomial = 7 // default
};
*/

static void prv_display_start(void) {
  // Hold LCD on
  // GPIO_WriteBit(DISP_LCD_GPIO, DISP_PIN_LCD, Bit_SET);
  BOARD_DISP_DISP_PORT->OUTSET = (1 << BOARD_DISP_DISP_PIN);
}

// Clear-all mode is entered by sending 0x04 to the panel
void display_clear(void) {
  prv_enable_display_access();

  prv_display_write_byte(DISP_MODE_CLEAR);
  prv_display_write_byte(0x00);

  prv_disable_display_access();
}

//! Static mode is entered by sending 0x00 to the panel
//! This stops any further updates being registered by
//! the display, preventing corruption on shutdown / boot
static void prv_display_enter_static(void) {
  prv_enable_display_access();

  prv_display_write_byte(DISP_MODE_STATIC);
  prv_display_write_byte(0x00);
  prv_display_write_byte(0x00);

  prv_disable_display_access();
}

// Helper to reverse command bytes
static uint8_t prv_reverse_bits(uint8_t input) {
  uint8_t result;
  __asm__ ("rev  %[result], %[input]\n\t"
           "rbit %[result], %[result]"
           : [result] "=r" (result)
           : [input] "r" (input));
  return result;
}

static void prv_display_start_write(void) {
  prv_enable_display_access();

  prv_display_write_byte(DISP_MODE_WRITE);
}

static void prv_display_write_line(uint8_t line_addr, const uint8_t *line) {
  // 1-indexed (ugh) 8bit line address (1-168)
  prv_display_write_byte(prv_reverse_bits(line_addr + 1));

  for (int i = 0; i < DISP_LINE_BYTES; ++i) {
    prv_display_write_byte(prv_reverse_bits(line[i]));
  }

  prv_display_write_byte(0x00);
}

static void prv_display_end_write(void) {
  prv_display_write_byte(0x00);


  prv_disable_display_access();
}

// Round a bit offset to a byte offset
static unsigned prv_round_to_byte(unsigned x) {
  return (x + 7) >> 3;
}

// Draw bitmap onto buffer.
static void prv_draw_bitmap(const uint8_t *bitmap, unsigned x_offset, unsigned y_offset,
                            unsigned width, unsigned height,
                            uint8_t buffer[DISP_ROWS][DISP_LINE_BYTES]) {
  // Need to convert offsets to bytes for the horizontal dimensions
  x_offset = prv_round_to_byte(x_offset);
  width = prv_round_to_byte(width);

  for (unsigned i = 0; i < height; i++) {
    memcpy(buffer[i + y_offset] + x_offset, bitmap + i * width, width);
  }
}

static void prv_display_buffer(uint8_t buffer[DISP_ROWS][DISP_LINE_BYTES]) {
  prv_display_start_write();
  for (int i = 0; i < DISP_ROWS; i++) {
    prv_display_write_line(i, buffer[i]);
  }
  prv_display_end_write();
}

void display_boot_splash(void) {
  return;
}

static void prv_set_bit(uint8_t x, uint8_t y, uint8_t buffer[DISP_ROWS][DISP_LINE_BYTES]) {
  buffer[y][x / 8] |= (1 << (x % 8));
}

static void prv_render_char(unsigned digit, uint8_t x_offset_bits, uint8_t y_offset,
                            uint8_t buffer[DISP_ROWS][DISP_LINE_BYTES]) {
  const unsigned char_rows = 18, char_cols = 9;
  const uint8_t * char_data = hex_digits_bits[digit];

  // Each character requires 2 bytes of storage
  for (unsigned y = 0; y < char_rows; y++) {
    unsigned cur_y = y_offset + y;
    uint8_t first_byte = char_data[2 * y];

    for (unsigned x = 0; x < char_cols; x++) {
      bool pixel;
      if (x < 8) { // Pixel is in first byte
        pixel = first_byte & (1 << x);
      }
      else { // Last pixel is in second byte
        pixel = char_data[2 * y + 1] & 1;
      }

      // The buffer is already all black, so just set the white pixels
      if (pixel) {
        prv_set_bit(x_offset_bits + x, cur_y, buffer);
      }
    }
  }
}

static void prv_draw_code(uint32_t code, uint8_t buffer[DISP_ROWS][DISP_LINE_BYTES]) {
  const unsigned y_offset = 116; // beneath sad face, above url
  unsigned x_offset = 28; // Aligned with sad face

  // Extract and print digits
  for (int i = 7; i >= 0; i--) {
    // Mask off 4 bits at a time
    uint32_t mask = (0xf << (i * 4));
    unsigned digit = ((code & mask) >> (i * 4));
    prv_render_char(digit, x_offset, y_offset, buffer);

    // Each character is 9px wide plus 2px of padding
    x_offset += 11;
  }
}

void display_error_code(uint32_t code) {
  return;
}

//! Do whatever is necessary to prevent visual artifacts when resetting
//! the watch.
void display_prepare_for_reset(void) {
  return;
}

//! Display the progress of a firmware update.
//!
//! The progress is expressed as a rational number less than or equal to 1.
//! When numerator == denominator, the progress indicator shows that the update
//! is complete.
void display_firmware_update_progress(uint32_t numerator, uint32_t denominator) {
  return;
}

void display_init(void) {
  return;
}

void display_deinit(void) {
  return;
}
