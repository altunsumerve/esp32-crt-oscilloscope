# Credits & Third-Party Licenses

This project stands on other people's work. Honest attribution below.

## Written for this project (ours)
The entire oscilloscope application in `src/main.c`:
- ADC acquisition on the second core with a lock-free double buffer
- Direct framebuffer rendering
- A small built-in 5x7 bitmap font
- Waveform generator with Amplitude / Frequency / Shape modes
- Mode switching via the on-board BOOT button
- On-screen measurements (VDC / Vpp / frequency)

## Third-party components we use
- **esp32_composite_video_lib** by aquaticus — generates the PAL/NTSC composite
  video signal from the ESP32 DAC. **License: GPL-3.0-or-later.**
  https://github.com/aquaticus/esp32_composite_video_lib
  We changed exactly one line: `DAC_CHANNEL_1` -> `DAC_CHANNEL_2`
  (our board's DAC1/GPIO25 was damaged, so video output uses DAC2/GPIO26).
- **LVGL** (v8.3) — MIT License. Required to compile the video library.
  https://github.com/lvgl/lvgl
- **ESP-IDF** — Apache-2.0. The build framework.

## Explored but NOT used in the final project
- bitluni/ESP32CompositeVideo
- Roger-random/ESP_8_BIT_composite

## Why this project is GPL-3.0
Because it links against and modifies the GPL-3.0 `esp32_composite_video_lib`,
the combined work is distributed under **GPL-3.0-or-later**. See LICENSE.
