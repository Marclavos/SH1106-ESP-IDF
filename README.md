# SH1106 OLED Graphics and Text Driver for ESP-IDF
A lightweight, highly optimized 2D graphics and text engine for SH1106 OLED displays using ESP32 and ESP-IDF. Supports strict 5x8 font rendering with dynamic line wrap, native UTF-8 Spanish characters (ñ/Ñ), custom-padded inverted text blocks, geometric primitives (rectangles, filled shapes), and custom bitmap drawing.

## Installation
1. Create a directory called `components` in your main project directory.
2. Change into the components directory.
3. Run git clone `https://github.com/Marclavos/SH1106-ESP-IDF.git`
4. Compile with `idf.py build`
