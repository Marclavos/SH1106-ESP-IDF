# SH1106 OLED Graphics and Text Driver for ESP-IDF
A lightweight, highly optimized 2D graphics and text engine for SH1106 OLED displays using ESP32 and ESP-IDF. Supports strict 5x8 font rendering with dynamic line wrap, native UTF-8 Spanish characters (ñ/Ñ), custom-padded inverted text blocks, geometric primitives (rectangles, filled shapes), and custom bitmap drawing.

## Features
* **Strict 5x8 Font Rendering:** Optimized loops that ignore empty padding columns to maximize CPU efficiency and rendering speed.
* **Smart Text Wrapping:** Automatic horizontal line breaks based on `SCREEN_WIDTH`.
* **Native UTF-8 Spanish Support:** Seamless handling of multibyte `ñ` and `Ñ` characters directly in strings (no escape codes needed).
* **Enhanced Inverted Backgrounds:** Line-by-line bounding boxes for inverted text with custom top-margin padding for polished UI menus.
* **2D Primitives & Bitmaps:** Built-in functions to draw pixels, rectangles, filled shapes, and custom monochrome bitmaps.

## Quick start example
Example provided on https://github.com/Marclavos/SH1106-ESP-IDF/SH1106/example/main.c

```c
#include "sh1106.h"

const uint8_t customBitmap = {
  // ......
};

void app_main(void)
{
    // Initialize I2C and the SH1106 display
    sh1106_init();

    // Draw a geometric frame
    sh1106_drawRect(0, 0, 128, 64, SH1106_COLOR_WHITE);

    // Draw a custom bitmap
    sh1106_drawBitmap(0, 0, customBitmap, 128, 64, SH1106_COLOR_WHITE);

    // Print standard and inverted text
    sh1106_drawString(4, 4, "Main Menu", SH1106_COLOR_WHITE, 1, false);
    sh1106_drawString(4, 20, "Option 1", SH1106_COLOR_WHITE, 1, true); // Inverted mode

    Flush buffer to screen
    sh1106_display();
}
```

## Installation
1. Create a directory called `components` in your main project directory.
2. Change into the components directory.
3. Run git clone `https://github.com/Marclavos/SH1106-ESP-IDF/SH1106/components.git`
4. Compile with `idf.py build`
