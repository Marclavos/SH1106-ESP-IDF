/* Basic SH1106 Library based on the I2C protocol
 * sh1106.c
 * Source: https://docs.espressif.com/projects/esp-idf/en/v4.3/esp32/api-reference/peripherals/i2c.html
 * */

#include "sh1106.h"
#include "ascii8x8.h"

#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include <driver/i2c.h>
// #include <driver/i2c_master.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <sys/types.h>

// extern i2c_master_dev_handle_t dev_handle;
static uint8_t frame_buffer[SCREEN_HEIGHT / 8][SCREEN_WIDTH] = {0};

/* Deprecated for v7.x.x */
void sh1106_write_command(uint8_t cmd_val)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, SLAVE_ADDR | I2C_MASTER_WRITE, true);  // Slave address, ensure ACK

    i2c_master_write_byte(cmd, 0x00, true);  // Control byte
    i2c_master_write_byte(cmd, cmd_val, true);  // Command byte

    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_PORT, cmd, pdMS_TO_TICKS(10));
    i2c_cmd_link_delete(cmd);
}

// void sh1106_write_command(uint8_t cmd_val)
// {
//     uint8_t buffer[2] = {0x00, cmd_val};  // Control byte, Command
//     i2c_master_transmit(dev_handle, buffer, sizeof(buffer), pdMS_TO_TICKS(10));
// }

/* Deprecated for v7.x.x */
void sh1106_write_data(uint8_t *data, size_t size)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, SLAVE_ADDR | I2C_MASTER_WRITE, true);  // Slave address, ensure ACK

    i2c_master_write_byte(cmd, 0x40, true);  // Data mode (0x40)
    i2c_master_write(cmd, data, size, true);  // Data

    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_PORT, cmd, pdMS_TO_TICKS(10));
    i2c_cmd_link_delete(cmd);
}

// void sh1106_write_data(uint8_t *data, size_t size)
// {
//     uint8_t *data_buf = malloc(size + 1);
//     if (data_buf == NULL) return;
//
//     data_buf[0] = 0x40;  // Data mode (0x40)
//     memcpy(&data_buf[1], data, size);
//
//     i2c_master_transmit(dev_handle, data_buf, size + 1, pdMS_TO_TICKS(10));
//
//     free(data_buf);
// }

void sh1106_init(void)
{
    sh1106_write_command(0xAE);  // Turn off the display for configuration

    sh1106_write_command(0xD5);  // Configure oscillator frequency
    sh1106_write_command(0x80);  // Default ratio

    sh1106_write_command(0xA8);  // Configure multiplex ratio (number of fisical mapped lines)
    sh1106_write_command(0x3F);  // 64 lines (0x3F)

    sh1106_write_command(0xD3);  // Configure display offset
    sh1106_write_command(0x00);  // Vertical displacement to 0

    sh1106_write_command(0x40);  // Set the start line of the RAM to 0

    sh1106_write_command(0xAD);  // Activate the charge pump
    sh1106_write_command(0x8B);  // Activate the internal voltage multiplier

    sh1106_write_command(0xA1);  // Segment addressing

    sh1106_write_command(0xC8);  // COM output scanning

    sh1106_write_command(0xDA);  // COM pins configuration (hardware)
    sh1106_write_command(0x12);

    sh1106_write_command(0x81);  // Configure contrast
    sh1106_write_command(0xBF);  // Brightness (0x00 - 0xFF)

    sh1106_write_command(0xD9);  // Diode pre-charge period
    sh1106_write_command(0x22);

    sh1106_write_command(0xDB);  // Voltage selection level VCOMH
    sh1106_write_command(0x40);

    sh1106_write_command(0xA4);  // Use RAM

    sh1106_write_command(0xA6);  // Set normal mode (1 on, 0 off)

    vTaskDelay(pdMS_TO_TICKS(100));  // Wait for internal 9V voltages to stabilize

    sh1106_update();

    sh1106_write_command(0xAF);  // Turn on the display
}

/* Deprecated for v7.x.x */
void sh1106_update(void)
{
    for (uint8_t page = 0; page < SCREEN_HEIGHT / 8; page++)
    {
        /* Move cursor and set page */
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, SLAVE_ADDR | I2C_MASTER_WRITE, true);

        i2c_master_write_byte(cmd, 0x00, true);  // Control byte
        i2c_master_write_byte(cmd, 0xB0 + page, true);  // Select page
        i2c_master_write_byte(cmd, 0x02, true);  // Select position (x = 0) OFFSET = 2
        i2c_master_write_byte(cmd, 0x10, true);

        i2c_master_stop(cmd);
        i2c_master_cmd_begin(I2C_MASTER_PORT, cmd, pdMS_TO_TICKS(10));
        i2c_cmd_link_delete(cmd);

        /* Send data */
        cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, SLAVE_ADDR | I2C_MASTER_WRITE, true);

        i2c_master_write_byte(cmd, 0x40, true);  // Data mode (0x40)
        i2c_master_write(cmd, frame_buffer[page], SCREEN_WIDTH, true);  // Send data

        i2c_master_stop(cmd);
        i2c_master_cmd_begin(I2C_MASTER_PORT, cmd, pdMS_TO_TICKS(20)); // Don't change Ticks to < 20, ACK will not be recieved
        i2c_cmd_link_delete(cmd);

        vTaskDelay(pdMS_TO_TICKS(10));  // Don't change Ticks to < 10, is not enough to SH1106 refresh rate
    }
}

// void sh1106_update(void)
// {
//     uint8_t data_buf[31];
//     data_buf[0] = 0x40;
//
//     for (int i = 1; i < 31; i++)
//     {
//         data_buf[i] = 0x00;
//     }
//
//     for (uint8_t page = 0; page < 8; page++)
//     {
//         uint8_t cmd_buf[4] = {0x00, 0xB0 + page, 0x02, 0x10};
//         i2c_master_transmit(dev_handle, cmd_buf, 4, pdMS_TO_TICKS(1));
//
//         i2c_master_transmit(dev_handle, data_buf, 31, pdMS_TO_TICKS(1));
//         i2c_master_transmit(dev_handle, data_buf, 31, pdMS_TO_TICKS(1));
//         i2c_master_transmit(dev_handle, data_buf, 31, pdMS_TO_TICKS(1));
//         i2c_master_transmit(dev_handle, data_buf, 31, pdMS_TO_TICKS(1));
//         i2c_master_transmit(dev_handle, data_buf, 9, pdMS_TO_TICKS(1));
//     }
// }

void sh1106_clear(void)
{
    memset(frame_buffer, 0, sizeof(frame_buffer));
    sh1106_update();
}

void sh1106_drawPixel(uint8_t x, uint8_t y, uint8_t color)
{
    if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) return;

    /* Get page and bit from coords */
    uint8_t page = y / 8;
    uint8_t bit = y % 8;

    if (color == SH1106_COLOR_WHITE)
    {
        frame_buffer[page][x] |= (1 << bit);  // Turn ON (OR)
    } else
    {
        frame_buffer[page][x] &= ~(1 << bit);  // Turn OFF (AND)
    }
}

void sh1106_drawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color)
{
    if (width == 0 || height == 0) return;

    int16_t x_end = x + width - 1;
    int16_t y_end = y + height - 1;

    for (int16_t i = x; i <= x_end; i++)
    {
        if (i >= 0 && i < SCREEN_WIDTH)
        {
            if (y < SCREEN_HEIGHT) sh1106_drawPixel((uint8_t)i, y, color);
            if (y_end >= 0 && y_end < SCREEN_HEIGHT) sh1106_drawPixel((uint8_t)i, (uint8_t)y_end, color);
        }
    }

    for (int16_t j = y; j <= y_end; j++)
    {
        if (j >= 0 && j < SCREEN_HEIGHT)
        {
            if (x < SCREEN_WIDTH) sh1106_drawPixel(x, (uint8_t)j, color);
            if (x_end >= 0 && x_end < SCREEN_WIDTH) sh1106_drawPixel((uint8_t)x_end, (uint8_t)j, color);
        }
    }
}

void sh1106_drawFillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color)
{
    if (width == 0 || height == 0) return;

    int16_t max_x = x + width;
    int16_t max_y = y + height;

    if (max_x > SCREEN_WIDTH) max_x = SCREEN_WIDTH;
    if (max_y > SCREEN_HEIGHT) max_y = SCREEN_HEIGHT;

    for (uint16_t j = y; j < max_y; j++)
    {
        for (uint16_t i = x; i < max_x; i++)
        {
            sh1106_drawPixel((uint8_t)i, (uint8_t)j, color);
        }
    }
}

void sh1106_drawBitmap(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t width, uint8_t height, uint8_t color)
{
    int16_t byteWidth = (width + 7) / 8;

    for (int16_t j = 0; j < height; j++)  // Y
    {
        for (int16_t i = 0; i< width; i++)  // X
        {
            int16_t pixel_x = x + i;
            int16_t pixel_y = y + j;
            if (pixel_x >= 0 && pixel_x < SCREEN_WIDTH && pixel_y >= 0 && pixel_y < SCREEN_HEIGHT)
            {
                if (bitmap[j * byteWidth + (i / 8)] & (1 << (7 - (i % 8))))  // Search the exact bit in the array
                {
                    sh1106_drawPixel((uint8_t)pixel_x, (uint8_t)pixel_y, color);
                }
                else
                {
                    sh1106_drawPixel((uint8_t)pixel_x, (uint8_t)pixel_y, !color);
                }
            }
        }
    }
}

void sh1106_drawChar(uint8_t x, uint8_t y, char c, uint8_t color, uint8_t scale)
{
    uint8_t ascii_val = (uint8_t)c;

    if ((ascii_val < 32 || ascii_val > 126) && (ascii_val != 164 && ascii_val != 165)) return;
    if (scale == 0) scale = 1;

    uint16_t char_index;
    if (ascii_val == 164) char_index = 95;
    else if (ascii_val == 165) char_index = 96;
    else char_index = ascii_val - 32;

    /* Rows */
    for (uint8_t j = 0; j < 8; j++)
    {
        uint8_t row_byte = Ascii8x8[char_index][j];  // Position

        /* Columns */
        for (uint8_t i = 0; i < 5; i++)
        {
            if ((row_byte & (1 << (7 - i))) != 0)
            {
                /* Draw pixel and apply scale */
                for (uint8_t sy = 0; sy < scale; sy++)
                {
                    for (uint8_t sx = 0; sx < scale; sx++)
                    {
                        int16_t pixel_x = x + (i * scale) + sx;
                        int16_t pixel_y = y + (j * scale) + sy;

                        if (pixel_x >= 0 && pixel_x < SCREEN_WIDTH && pixel_y >= 0 && pixel_y < SCREEN_HEIGHT)
                        {
                            sh1106_drawPixel((uint8_t)pixel_x, (uint8_t)pixel_y, color);
                        }
                    }
                }
            }
        }
    }
}

void sh1106_drawString(uint8_t x, uint8_t y, const char *str, uint8_t color, uint8_t scale, bool invert)
{
    if (str == NULL) return;
    if (scale == 0) scale = 1;

    uint8_t char_w = 5 * scale;  // Real char width
    uint8_t char_h = 8 * scale;  // Real char height

    uint8_t total_char_width = (5 + 1) * scale;  // 1 px margin
    uint8_t total_line_height = 10 * scale;  // 2px of line space

    /* Modify cursor position to apply the extra frame for inverted */
    uint8_t cursor_x = invert ? (x + scale) : x;
    uint8_t cursor_y = invert ? (y + scale) : y;
    bool build_bg = invert;

    while (*str)
    {
        /* n - ñ */
        uint8_t actual_char = *str;
        if (actual_char == 0xC3)
        {
            uint8_t next_byte = (uint8_t)*(str + 1);
            if (next_byte == 0xB1) actual_char = 164;
            else if (next_byte == 0x91) actual_char = 165;
        }

        /* Line jump */
        bool jump = false;  // Line jump flag

        if (actual_char == '\n')
        {
            str++;
            jump = true;
        }
        else if (cursor_x + char_w > SCREEN_WIDTH) jump = true;

        if (jump)  // Move cursor when jump
        {
            cursor_x = invert ? (x + scale) : x;
            cursor_y += total_line_height;
            build_bg = invert;
        }

        /* Vertical limit */
        if (cursor_y + char_h > SCREEN_HEIGHT) break;

        /* Inverted bg */
        if (build_bg)
        {
            uint16_t line_lenght = 0;
            const char *temp = str;
            uint16_t x_test = cursor_x;

            /* Simulate text movement to identify when the line jump happen */
            while (*temp && *temp != '\n' && (x_test + char_w <= SCREEN_WIDTH))
            {
                line_lenght ++;
                if ((uint8_t)*temp == 0xC3 && ((uint8_t)*(temp + 1) == 0xB1 || (uint8_t)*(temp + 1) == 0x91)) temp ++;
                x_test += total_char_width;
                temp ++;
            }

            /* Build the frame */
            if (line_lenght > 0)
            {
                uint8_t bg_width = (line_lenght * total_char_width) + scale;
                uint8_t bg_height = char_h + (3 * scale);  // Extra + 1 pixel on top

                sh1106_drawFillRect(cursor_x - scale, cursor_y - (2 * scale), bg_width, bg_height, color);
            }

            build_bg = false;
        }

        /* Draw char */
        sh1106_drawChar(cursor_x, cursor_y, actual_char, invert ? !color : color, scale);

        /* Move cursor */
        cursor_x += total_char_width;

        if (actual_char == 164 || actual_char == 165) str ++;
        str ++;
    }
}
