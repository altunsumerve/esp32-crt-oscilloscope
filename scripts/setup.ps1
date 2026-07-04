# Fetches the third-party components and applies the one-line DAC patch (Windows).
$ErrorActionPreference = "Stop"
Set-Location (Join-Path $PSScriptRoot "..")

git clone --depth 1 https://github.com/aquaticus/esp32_composite_video_lib.git components/esp32_composite_video_lib
git clone --depth 1 --branch release/v8.3 https://github.com/lvgl/lvgl.git components/lvgl
Copy-Item components/lvgl/lv_conf_template.h components/lv_conf.h

(Get-Content components/lv_conf.h) `
  -replace '#if 0 .*Set it to "1" to enable content.*', '#if 1' `
  -replace '#define LV_COLOR_DEPTH 16', '#define LV_COLOR_DEPTH 8' |
  Set-Content components/lv_conf.h

(Get-Content components/esp32_composite_video_lib/video.c) `
  -replace 'dac_output_enable\(DAC_CHANNEL_1\)', 'dac_output_enable(DAC_CHANNEL_2)' |
  Set-Content components/esp32_composite_video_lib/video.c

Write-Host "Setup done. Video output is on GPIO26 (DAC2)."
