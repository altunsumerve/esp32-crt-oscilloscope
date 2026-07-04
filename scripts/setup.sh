#!/usr/bin/env bash
# Fetches the third-party components and applies the one-line DAC patch.
set -e
cd "$(dirname "$0")/.."

git clone --depth 1 https://github.com/aquaticus/esp32_composite_video_lib.git components/esp32_composite_video_lib
git clone --depth 1 --branch release/v8.3 https://github.com/lvgl/lvgl.git components/lvgl
cp components/lvgl/lv_conf_template.h components/lv_conf.h

# lv_conf.h: enable it and use 8-bit colour
sed -i 's/#if 0 .*Set it to "1" to enable content.*/#if 1/' components/lv_conf.h
sed -i 's/#define LV_COLOR_DEPTH 16/#define LV_COLOR_DEPTH 8/' components/lv_conf.h

# Video output uses DAC2 (GPIO26); the library defaults to DAC1 (GPIO25).
sed -i 's/dac_output_enable(DAC_CHANNEL_1)/dac_output_enable(DAC_CHANNEL_2)/' \
    components/esp32_composite_video_lib/video.c

echo "Setup done. Video output is on GPIO26 (DAC2)."
