/* Basic SH1106 Library based on the I2C protocol
 * sh1106.h
 * Source: https://docs.espressif.com/projects/esp-idf/en/v4.3/esp32/api-reference/peripherals/i2c.html
 * */

#ifndef SH1106_H
#define SH1106_H

#include <stdint.h>
#include <stddef.h>

#define PIN_SDA 21
#define PIN_SCL 22
#define CLK_SPEED 100000    // 100kHz

/* Deprecated for v7.x.x */
#define SLAVE_ADDR (0x3C << 1)
#define I2C_MASTER_PORT 0

// #define SLAVE_ADDR 0x3C)

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define SH1106_COLOR_WHITE 1
#define SH1106_COLOR_BLACK 0

/* Fonts */
typedef struct
{
    uint16_t bitmap_offset;
    uint8_t width;
    uint8_t height;
    uint8_t x_advance;
    int8_t x_offset;
    int8_t y_offset;
} sh1106_char_t;

typedef struct
{
    const uint8_t *bitmap;
    const sh1106_char_t *glyphs;
    uint8_t first_char;
    uint8_t last_char;
    uint8_t font_height;
} sh1106_font_t;

void sh1106_write_command(uint8_t cmd_val);
void sh1106_write_data(uint8_t *data, size_t size);
void sh1106_init(void);
void sh1106_update(void);
void sh1106_clear(void);
void sh1106_drawPixel(uint8_t x, uint8_t y, uint8_t color);
void sh1106_drawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);
void sh1106_drawFillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);
void sh1106_drawBitmap(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t width, uint8_t height, uint8_t color);
void sh1106_drawChar(uint8_t x, uint8_t y, char c, uint8_t color, uint8_t scale);
void sh1106_drawString(uint8_t x, uint8_t y, const char *str, uint8_t color, uint8_t scale, bool invert);

#endif
